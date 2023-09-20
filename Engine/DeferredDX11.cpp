#include "pch.h"
#include "DeferredDX11.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "Component.h"
#include "UIRenderer.h"
#include "EngineSettings.h"
#include "RenderUtils.h"
#include "PostProcesses.h"

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

	// TO-DO: Init function desparately needed
	// TO-DO: Pack this data better
	//Setup
	m_DeferredRenderTargets.m_pRenderTargets = &GetRenderTargets();

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

	m_DeferredRenderTargets.m_PositionTarget		= m_pDevice->TryGetRenderTarget(2, true, desc1); // TO-DO : Fix this , Index 0 is guaranteed to exists by design, but is the final render target (swapchain buffer)
	m_DeferredRenderTargets.m_NormalTarget			= m_pDevice->TryGetRenderTarget(3, true, desc1);
	m_DeferredRenderTargets.m_TangentTarget			= m_pDevice->TryGetRenderTarget(4, true, desc1);
	m_DeferredRenderTargets.m_BinormalTarget		= m_pDevice->TryGetRenderTarget(5, true, desc1);
	m_DeferredRenderTargets.m_AlbedoTarget			= m_pDevice->TryGetRenderTarget(6, true, desc1);
	m_DeferredRenderTargets.m_MetalRoughnessTarget	= m_pDevice->TryGetRenderTarget(7, true, desc2);

	CreateNDCQuad(&m_pScreenQuadVertexBuffer, &m_pScreenQuadIndexBuffer, m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies);
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

	//Unhook render targets from material
	ExplicitlyUnbindingRenderTargets();

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
	
		//Unhook render targets from material
		ExplicitlyUnbindingRenderTargets(m_pDeferredLightingMaterial);
	}

	//**********************************************************************
	// 	   Postprocess
	//**********************************************************************
	{
		m_pPostProcessingPipeline->RunPostProcesses(m_pDevice, this);
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	//Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	{
		ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
		pContext->OMSetRenderTargets(1, &GetFinalTarget()->pRenderTargetView, nullptr);
		UIRenderer::Instance()->RenderUI();
	}

	//Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(EngineSettings::Instance()->GetIfVSync(), 0);
}
void DeferredDX11::ClearBuffers()
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Clear the main target buffer to a deep blue
	pContext->ClearRenderTargetView(GetFinalSceneTarget()->pRenderTargetView, &m_DefaultClearColor.x);
	//Clear the other buffers as well with black
	const DirectX::XMFLOAT4 black = { 0,0,0,0 };
	pContext->ClearRenderTargetView(GetFinalTarget()->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_PositionTarget->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_NormalTarget->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_TangentTarget->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_BinormalTarget->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_AlbedoTarget->pRenderTargetView, &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_MetalRoughnessTarget->pRenderTargetView, &black.x);
	//Clear the depth buffer to the max value
	pContext->ClearDepthStencilView(m_pDevice->GetDepthBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma region RenderHelpers
void DeferredDX11::SetupTargetsDeferredFirstPass()
{
	//Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTargs[] =
	{
		//Used for storing position, normals etc.
		m_DeferredRenderTargets.m_PositionTarget->pRenderTargetView,
		m_DeferredRenderTargets.m_NormalTarget->pRenderTargetView,
		m_DeferredRenderTargets.m_TangentTarget->pRenderTargetView,
		m_DeferredRenderTargets.m_BinormalTarget->pRenderTargetView,
		m_DeferredRenderTargets.m_AlbedoTarget->pRenderTargetView,
		m_DeferredRenderTargets.m_MetalRoughnessTarget->pRenderTargetView,
	};
	pContext->OMSetRenderTargets(m_NrBasisRenderTargets, renderTargs, m_pDevice->GetDepthBuffer());
}
void DeferredDX11::SetupTargetsDeferredSecondPass()
{
	//Set the render target
	m_pDevice->GetDeviceContext()->OMSetRenderTargets(1, &GetFinalSceneTarget()->pRenderTargetView, m_pDevice->GetDepthBuffer());
}
#pragma endregion

Material* DeferredDX11::GetDeferredLightingPassMaterial() const
{
	return m_pDeferredLightingMaterial;
}
