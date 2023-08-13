#include "pch.h"
#include "DeferredDX11.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "Component.h"
#include "UIRenderer.h"
#include "EngineSettings.h"

DeferredDX11::DeferredDX11(EngineDevice* device) 
	:Renderer(device)
	, m_pDeferredLightingMaterial{ nullptr }
	, m_pScreenQuadVertexBuffer{ nullptr }
	, m_pScreenQuadIndexBuffer{ nullptr }
	, m_ScreenQuadNrVerticies{ 0 }
	, m_ScreenQuadNrIndicies{ 0 }
{
	m_RenderType = RenderType::deferred;

	m_pDeferredLightingMaterial = new Material(L"..\\Resources\\Shaders\\DeferredLighting.fx", RenderType::lightingPass);
	m_pDeferredLightingMaterial->InitShader(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext());

	CreateNDCQuad();
}
DeferredDX11::~DeferredDX11() 
{
	SafeRelease(m_pScreenQuadVertexBuffer);
	SafeRelease(m_pScreenQuadIndexBuffer);
	SafeDelete(m_pDeferredLightingMaterial);
}

void DeferredDX11::Render()
{
	//No camera == no rendering
	if (!m_pRenderingCamera)
	{
		return;
	}

	ClearBuffers();

	//**********************************************************************
	// 	   Main render loop
	//**********************************************************************
	for (int i = 0; i < m_Renderables.size(); ++i)
	{
		if (m_Renderables[i]->IsVisible())
		{
			Material* pMaterial = m_Renderables[i]->GetMaterial();
			//Set the parameters
			pMaterial->UpdateParameterValues(m_pDevice);
			//Update matrices
			UpdateMatrices(m_Renderables[i]);
			//Set the render target
			SetupTargetsDeferredFirstPass();
			//First pass
			m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
		}
	}

	//**********************************************************************
	// 	   Final Render for deferred
	//**********************************************************************
	{
		//Set the material buffers
		m_pDeferredLightingMaterial->UpdateParameterValues(m_pDevice);
		if (m_UpdateLighting)
		{
			//Update the lights
			UpdateLights(m_pDeferredLightingMaterial);
		}
		//Update view matrix
		UpdateMatrices(m_pDeferredLightingMaterial);
		//Set the render target
		SetupTargetsDeferredSecondPass();
		//Second/Deferred pass
		//Select which vertex buffer to display
		const UINT stride = sizeof(QuadVertex);
		const UINT offset = 0;
		ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
		pContext->IASetVertexBuffers(0, 1, &m_pScreenQuadVertexBuffer, &stride, &offset);

		//Set the index buffer to active in the input assembler so it can be rendered.
		pContext->IASetIndexBuffer(m_pScreenQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pDeferredLightingMaterial->Render(pContext, m_ScreenQuadNrIndicies, 0);
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	//Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	UIRenderer::Instance()->RenderUI();

	//Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(EngineSettings::Instance()->GetIfVSync(), 0);

	//Unhook render targets from material
	ExplicitlyUnbindingRenderTargets();
}
void DeferredDX11::ClearBuffers()
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Clear the main target buffer to a deep blue
	pContext->ClearRenderTargetView(m_pDevice->GetRenderTarget(0)->pRenderTargetView, &m_DefaultClearColor.x);
	//Clear the other buffers as well with black
	RenderTarget* pRenderTarget;
	ID3D11RenderTargetView* pViewToClear;
	int index = 1;
	do
	{
		pRenderTarget = m_pDevice->GetRenderTarget(index);
		if (pRenderTarget)
		{
			pViewToClear = pRenderTarget->pRenderTargetView;
			const DirectX::XMFLOAT4 black = { 0,0,0,0 };
			pContext->ClearRenderTargetView(pViewToClear, &black.x);
			//Get the next render target
			++index;
		}
	} while (pRenderTarget && pViewToClear);
	//Clear the depth buffer to the max value
	pContext->ClearDepthStencilView(m_pDevice->GetDepthBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma region RenderHelpers
void DeferredDX11::SetupTargetsDeferredFirstPass() const
{
	//Setup
	D3D11_TEXTURE2D_DESC desc1;
	ZeroMemory(&desc1, sizeof(desc1));
	desc1.Width = m_pDevice->GetDefaultWidth();
	desc1.Height = m_pDevice->GetDefaultHeight();
	desc1.MipLevels = 1;
	desc1.ArraySize = 1;
	desc1.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc1.SampleDesc.Count = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.Usage = D3D11_USAGE_DEFAULT;
	desc1.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc1.CPUAccessFlags = 0;
	desc1.MiscFlags = 0;
	D3D11_TEXTURE2D_DESC desc2;
	ZeroMemory(&desc2, sizeof(desc2));
	desc2.Width = m_pDevice->GetDefaultWidth();
	desc2.Height = m_pDevice->GetDefaultHeight();
	desc2.MipLevels = 1;
	desc2.ArraySize = 1;
	desc2.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
	desc2.SampleDesc.Count = 1;
	desc2.SampleDesc.Quality = 0;
	desc2.Usage = D3D11_USAGE_DEFAULT;
	desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2.CPUAccessFlags = 0;
	desc2.MiscFlags = 0;

	//Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTargs[] =
	{
		//Used for storing position, normals etc.
		m_pDevice->TryGetRenderTarget(1, true, desc1)->pRenderTargetView,		// TO-DO : Fix this , Index 0 is guaranteed to exists by design, but is the final render target (swapchain buffer)
		m_pDevice->TryGetRenderTarget(2, true, desc1)->pRenderTargetView,
		m_pDevice->TryGetRenderTarget(3, true, desc1)->pRenderTargetView,
		m_pDevice->TryGetRenderTarget(4, true, desc1)->pRenderTargetView,
		m_pDevice->TryGetRenderTarget(5, true, desc1)->pRenderTargetView,
		m_pDevice->TryGetRenderTarget(6, true, desc2)->pRenderTargetView,
	};
	pContext->OMSetRenderTargets(m_NrBasisRenderTargets, renderTargs, m_pDevice->GetDepthBuffer());
}
void DeferredDX11::SetupTargetsDeferredSecondPass() const
{
	//Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTarg = m_pDevice->GetRenderTarget(0)->pRenderTargetView; //TO-DO : Fix this , Index 0 is guaranteed to exists by design, but is the final render target (swapchain buffer)
	pContext->OMSetRenderTargets(1, &renderTarg, m_pDevice->GetDepthBuffer());
}

void DeferredDX11::CreateNDCQuad()
{
	ID3D11Device* pDevice = EngineManager::Instance()->GetDevice()->GetDevice();

	//***********************************************************************************
	// 	   Quad
	//***********************************************************************************
	D3D11_SUBRESOURCE_DATA vertexMS, indexMS;
	D3D11_BUFFER_DESC vertexBD, indexBD;
	HRESULT result;

	std::vector<QuadVertex> vertices{};
	std::vector<unsigned int> indices{};

	//Create screen quad for deferred
	vertices.resize(4);
	indices.resize(6);

	//Position coordinates specified in NDC space.
	vertices[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } };
	vertices[3] = { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	m_ScreenQuadNrVerticies = (UINT)vertices.size();
	m_ScreenQuadNrIndicies = (UINT)indices.size();

	//Vertex
	vertexBD.Usage = D3D11_USAGE_DEFAULT;
	vertexBD.ByteWidth = sizeof(QuadVertex) * m_ScreenQuadNrVerticies;
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.CPUAccessFlags = 0;
	vertexBD.MiscFlags = 0;
	vertexBD.StructureByteStride = sizeof(QuadVertex);

	//Give the subresource structure a pointer to the index data.
	vertexMS.pSysMem = &vertices[0];
	vertexMS.SysMemPitch = 0;
	vertexMS.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&vertexBD, &vertexMS, &m_pScreenQuadVertexBuffer);
	if (FAILED(result))
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Renderer: Failed to create vertexbuffer for defered render quad."));
		return;
	}

	//Index
	indexBD.Usage = D3D11_USAGE_DEFAULT;
	indexBD.ByteWidth = sizeof(unsigned int) * m_ScreenQuadNrIndicies;
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.CPUAccessFlags = 0;
	indexBD.MiscFlags = 0;
	indexBD.StructureByteStride = sizeof(unsigned int);

	//Give the subresource structure a pointer to the index data.
	indexMS.pSysMem = &indices[0];
	indexMS.SysMemPitch = 0;
	indexMS.SysMemSlicePitch = 0;
	result = pDevice->CreateBuffer(&indexBD, &indexMS, &m_pScreenQuadIndexBuffer);
	if (FAILED(result))
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Renderer: Failed to create indexbuffer for defered render quad."));
		return;
	}
}

void DeferredDX11::ExplicitlyUnbindingRenderTargets() const
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11ShaderResourceView* srvs = nullptr;
	pContext->PSSetShaderResources(0, 1, &srvs);
	ID3D11RenderTargetView* nullRTV = nullptr;
	pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	m_pDeferredLightingMaterial->ExplicitlyUnbindingResources(m_pDevice);
}
#pragma endregion

Material* DeferredDX11::GetDeferredLightingPassMaterial() const
{
	return m_pDeferredLightingMaterial;
}
