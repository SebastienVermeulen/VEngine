#include "pch.h"
#include "Renderer.h"
#include "Singleton.h"
#include "UIRenderer.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "Object.h"
#include "Renderable.h"
#include "Component.h"
#include "Transform.h"
#include "Light.h"
#include "Camera.h"
#include "RendererWidget.h"

EngineDevice* Renderer::m_pDevice = nullptr;

Renderer::Renderer(EngineDevice* pDevice)
	:m_pRendererWidget{new RendererWidget(this)}
	, m_Renderables {}
	, m_Lights{}
	, m_pCameraList{}
	, m_pRenderingCamera{}
	, m_pDeferredLightingMaterial{}
	, m_pScreenQuadVertexBuffer{}
	, m_pScreenQuadIndexBuffer{}
	, m_ScreenQuadNrVerticies{}
	, m_ScreenQuadNrIndicies{}
	, m_DefaultClearColor{ DirectX::XMFLOAT4{0.0f, 0.2f, 0.4f, 0.0f} }
	, m_RenderType{RenderType::forwards}
	, m_NrBasisRenderTargets{6}
	, m_VSync{true} // Set to true atm. as the engine can get "crazy" fps and I don't want to create fire 
	, m_UpdateLighting{false}
{
    m_pDevice = pDevice;

	m_pDeferredLightingMaterial = new Material(L"..\\Resources\\Shaders\\DeferredLighting.fx", RenderType::lightingPass);
	m_pDeferredLightingMaterial->InitShader(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext());

	m_Lights.reserve(Light::GetMaxNrLights());

	CreateNDCQuad();
}
Renderer::~Renderer()
{
	SafeDelete(m_pRendererWidget);
	for (int i = 0; i < m_Renderables.size(); ++i)
	{
		m_Renderables[i] = nullptr;
	}
	m_Renderables.clear();
	for (int i = 0; i < m_pCameraList.size(); ++i)
	{
		m_pCameraList[i] = nullptr;
	}
	m_pCameraList.clear();
	SafeRelease(m_pScreenQuadVertexBuffer);
	SafeRelease(m_pScreenQuadIndexBuffer);
	SafeDelete(m_pDeferredLightingMaterial);
}

void Renderer::Render()
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
		Material* pMaterial = m_Renderables[i]->GetMaterial();
		if (pMaterial->GetIfDeferred() == RenderType::deferred && m_RenderType == RenderType::deferred)
		{
			//Set the parameters
			pMaterial->UpdateParameterValues(m_pDevice);
			//Update matrices
			UpdateMatrices(m_Renderables[i]);
			//Set the render target
			SetupTargetsDeferredFirstPass();
			//First pass
			m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
		}
		else if (pMaterial->GetIfDeferred() == RenderType::forwards && m_RenderType == RenderType::forwards)
		{
			//Set the parameters
			pMaterial->UpdateParameterValues(m_pDevice);
			if (m_UpdateLighting)
			{
				//Update the lights
				UpdateLights(m_Renderables[i]->GetMaterial());
			}
			//Update matrices
			UpdateMatrices(m_Renderables[i]);
			//Set the render target
			SetupTargetsForwardsPass();
			//Forwards pass
			m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
		}
	}

	//**********************************************************************
	// 	   Final Render for deferred
	//**********************************************************************
	if (m_RenderType == RenderType::deferred)
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
		RenderDeferred();
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	//Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	UIRenderer::Instance()->RenderUI();

	//Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(m_VSync, 0);

	if (m_RenderType == RenderType::deferred)
	{
		//Unhook render targets from material
		ExplicitlyUnbindingRenderTargets();
	}
}
void Renderer::ClearBuffers()
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
void Renderer::RenderDeferred() 
{
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

#pragma region RenderablesStorage
void Renderer::AddRenderable(Component* pRenderable)
{
	//Try to cast to a camera
	//If it is a camera add to camera list and skip rest
	if (dynamic_cast<Camera*>(pRenderable))
	{
		NewCameraAddedToScene((Camera*)pRenderable);
		return;
	}
	//Try to cast to a light
	//If it is a light add to light list and skip rest
	if (dynamic_cast<Light*>(pRenderable))
	{
		NewLightAddedToScene((Light*)pRenderable);
		return;
	}

	if (std::find(m_Renderables.begin(), m_Renderables.end(), pRenderable) == m_Renderables.end())
	{
		m_Renderables.push_back(pRenderable);
	}
}
void Renderer::RemoveRenderable(Component* pRenderable)
{
	//Try to cast to a camera
	//If it is a camera remove from camera list and skip rest
	if (dynamic_cast<Camera*>(pRenderable))
	{
		NewCameraAddedToScene((Camera*)pRenderable);
		return;
	}

	if (m_Renderables.size())
	{
		std::vector<Component*>::iterator it = std::find(m_Renderables.begin(), m_Renderables.end(), pRenderable);
		if (it != m_Renderables.end())
		{
			m_Renderables.erase(it);
		}
	}
}

void Renderer::NewCameraAddedToScene(Camera* pCamera)
{
	if (std::find(m_pCameraList.begin(), m_pCameraList.end(), pCamera) == m_pCameraList.end())
	{
		m_pCameraList.push_back(pCamera);
	}

	if (!m_pRenderingCamera)
	{
		m_pRenderingCamera = pCamera;
	}
}
void Renderer::CameraRemovedFromScene(Camera* pCamera)
{
	if (m_pCameraList.size())
	{
		std::vector<Camera*>::iterator it = std::find(m_pCameraList.begin(), m_pCameraList.end(), pCamera);
		if (it != m_pCameraList.end())
		{
			m_pCameraList.erase(it);
		}
	}
}
void Renderer::NewLightAddedToScene(Light* pLight)
{
	if (std::find(m_Lights.begin(), m_Lights.end(), pLight) == m_Lights.end())
	{
		m_Lights.push_back(pLight);
		m_UpdateLighting = true;
	}
}
void Renderer::LightRemovedFromScene(Light* pLight)
{
	if (m_Lights.size())
	{
		std::vector<Light*>::iterator it = std::find(m_Lights.begin(), m_Lights.end(), pLight);
		if (it != m_Lights.end())
		{
			m_Lights.erase(it);
			m_UpdateLighting = true;
		}
	}
}
#pragma endregion

#pragma region RenderHelpers
void Renderer::UpdateMatrices(Component* pRenderable) const
{
	MatrixRenderBuffer buffer;

	buffer.world = pRenderable->Getobject()->GetTransform()->GetWorld();
	buffer.view = m_pRenderingCamera->GetViewMatrix();
	buffer.inverseView = m_pRenderingCamera->GetViewInverseMatrix();
	buffer.projection = m_pRenderingCamera->GetProjectionMatrix();
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));
	buffer.worldViewProj = worldViewProj;

	pRenderable->GetMaterial()->UpdateMatrix(buffer);
}
void Renderer::UpdateMatrices(Material* pMaterial) const
{
	MatrixRenderBuffer buffer;

	DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
	buffer.view = m_pRenderingCamera->GetViewMatrix();
	buffer.inverseView = m_pRenderingCamera->GetViewInverseMatrix();
	buffer.projection = m_pRenderingCamera->GetProjectionMatrix();
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));
	buffer.worldViewProj = worldViewProj;

	pMaterial->UpdateMatrix(buffer);
}
void Renderer::UpdateLights(Material* pMaterial)
{
	//TO-DO: don't include inactive ones
	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShaderLight> lightsStructure{};
	lightsStructure.reserve(maxNrLights);
	for (int i{}; i < maxNrLights; ++i)
	{
		if (m_Lights.size() > i && m_Lights[i]->ShouldRender())
		{
			//Add the light to structure for easy use
			Transform* pTransform = m_Lights[i]->Getobject()->GetTransform();
			DirectX::XMFLOAT3 pos = pTransform->GetPosition();
			DirectX::XMFLOAT3 dir = pTransform->GetForward();
			ShaderLight light{ 
				DirectX::XMFLOAT4{pos.x, pos.y, pos.z, 1.0f},
				DirectX::XMFLOAT4{dir.x, dir.y, dir.z, 1.0f},
				m_Lights[i]->GetColor(),
				m_Lights[i]->GetIntensity(),
				m_Lights[i]->GetLightType(),
				m_Lights[i]->GetIsEnabled() };
			lightsStructure.push_back(light);
		}
		else
		{
			ShaderLight light{};
			lightsStructure.push_back(light);
		}
	}
	pMaterial->UpdateMaterialLighting(m_pDevice->GetDeviceContext(), lightsStructure);
}

void Renderer::SetupTargetsDeferredFirstPass() const
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
void Renderer::SetupTargetsDeferredSecondPass() const
{
	//Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTarg = m_pDevice->GetRenderTarget(0)->pRenderTargetView; //TO-DO : Fix this , Index 0 is guaranteed to exists by design, but is the final render target (swapchain buffer)
	pContext->OMSetRenderTargets(1, &renderTarg, m_pDevice->GetDepthBuffer());
}
void Renderer::SetupTargetsForwardsPass() const
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Index 0 is guaranted to exists by design, but is the final render target (swapchain buffer)
	pContext->OMSetRenderTargets(1, &m_pDevice->GetRenderTarget(0)->pRenderTargetView, m_pDevice->GetDepthBuffer());
}
void Renderer::ExplicitlyUnbindingRenderTargets() const
{
	//DX11 is lazy by design, once needed it will implicitly unbind
	//Good practice is to unbind yourself once you are done so the device doesn't have to
	//Data might change from input to output or opposite then you need to act
	//https://stackoverflow.com/questions/52966262/id3dx11effectshaderresourcevariablesetresourcenull-cant-unbind-resources
	//https://www.gamedev.net/forums/topic/601013-directx11-multiple-render-target/
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11ShaderResourceView* srvs = nullptr;
	pContext->PSSetShaderResources(0, 1, &srvs);
	ID3D11RenderTargetView* nullRTV = nullptr; 
	pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	m_pDeferredLightingMaterial->ExplicitlyUnbindingResources(m_pDevice);
}

void Renderer::CreateNDCQuad()
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
		//TO-DO: make logger
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
		//TO-DO: make logger
		return;
	}
}
#pragma endregion

Material* Renderer::GetDeferredLightingPassMaterial() const
{
	return m_pDeferredLightingMaterial;
}
Camera* Renderer::GetRenderingCamera() const
{
	return m_pRenderingCamera;
}