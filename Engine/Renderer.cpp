#include "pch.h"
#include "Renderer.h"
#include "Singleton.h"
#include "UIRenderer.h"
#include "EngineManager.h"
#include "EngineSettings.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "Object.h"
#include "Renderable.h"
#include "Component.h"
#include "Transform.h"
#include "Light.h"
#include "Camera.h"
#include "RendererWidget.h"
#include "PostProcesses.h"
#include "ShadowManager.h"
#include "ShadowCasting.h"

EngineDevice* Renderer::m_pDevice = nullptr;
EngineSettings* Renderer::m_pEngineSettings = nullptr;

Renderer::Renderer(EngineDevice* pDevice)
	:m_pRendererWidget{new RendererWidget()}
	, m_Renderables {}
	, m_Lights{}
	, m_pCameraList{}
	, m_pRenderingCamera{}
	, m_pShadowManager{}
	, m_pShadowCasting{new ShadowCasting()}
	, m_pPostProcessingPipeline{new PostProcessPipeline()}
	, m_DefaultClearColor{ DirectX::XMFLOAT4{0.0f, 0.2f, 0.4f, 0.0f} }
	, m_NrBasisRenderTargets{5} // TO-DO: Uh oh, what was I thinking
	, m_UpdateLighting{false}
{
    m_pDevice = pDevice;
	m_pEngineSettings = EngineSettings::Instance();
	m_pShadowCasting->GetShadowMaterial()->InitShader(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext());
}
Renderer::~Renderer()
{
	SafeDelete(m_pShadowCasting);
	SafeDelete(m_pPostProcessingPipeline);
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
}

void Renderer::Init()
{
	// Reserve our lights
	m_Lights.reserve(Light::GetMaxNrLights());

	// Make sure the postprocessing pass also gets initialized
	m_pPostProcessingPipeline->Init(m_pDevice);
}

void Renderer::AllocatePromisedTargets()
{
	m_RenderTargets.m_FinalTarget = m_pDevice->GetSwapChainTarget();

	// TO-DO: Pack this data better
	// Setup
	D3D11_TEXTURE2D_DESC desc1;
	ZeroMemory(&desc1, sizeof(desc1));
	desc1.Width = m_pDevice->GetDefaultWidth();
	desc1.Height = m_pDevice->GetDefaultHeight();
	desc1.MipLevels = 1;
	desc1.ArraySize = 1;
	desc1.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc1.SampleDesc.Count = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.Usage = D3D11_USAGE_DEFAULT;
	desc1.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc1.CPUAccessFlags = 0;
	desc1.MiscFlags = 0;

	m_RenderTargets.m_FinalSceneTarget = m_pDevice->TryGetRenderTarget(true, desc1);

	m_RenderTargets.m_DepthBuffer = m_pDevice->TryGetDepthStencil(false);
}
void Renderer::CleanPointersToTargets()
{
	m_RenderTargets.m_FinalTarget = nullptr;
	m_RenderTargets.m_FinalSceneTarget = nullptr;
	m_RenderTargets.m_DepthBuffer = nullptr;
}

void Renderer::OponActivation()
{
	AllocatePromisedTargets();
}
void Renderer::OponDeactivation()
{
	CleanPointersToTargets();
}

#pragma region RenderablesStorage
void Renderer::AddRenderable(Component* pRenderable)
{
	EngineManager* pEngineManager = EngineManager::Instance();

	//Try to cast to a camera
	//If it is a camera add to camera list and skip rest
	if (dynamic_cast<Camera*>(pRenderable))
	{
		for (RenderType type : pRenderable->GetRenderTypes()) 
		{
			pEngineManager->GetRenderer(type)->NewCameraAddedToScene((Camera*)pRenderable);
		}
		return;
	}
	//Try to cast to a light
	//If it is a light add to light list and skip rest
	if (dynamic_cast<Light*>(pRenderable))
	{
		for (RenderType type : pRenderable->GetRenderTypes())
		{
			pEngineManager->GetRenderer(type)->NewLightAddedToScene((Light*)pRenderable);
		}
		return;
	}

	if (std::find(m_Renderables.begin(), m_Renderables.end(), pRenderable) == m_Renderables.end())
	{
		for (RenderType type : pRenderable->GetRenderTypes())
		{
			pEngineManager->GetRenderer(type)->GetRenderables().push_back(pRenderable);
		}
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
	MatrixTransformationContainer buffer;
	buffer.MarkAllForUpdate();

	buffer.world = pRenderable->GetObject()->GetTransform()->GetWorld();
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
	MatrixTransformationContainer buffer;
	buffer.MarkAllForUpdate();

	DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
	buffer.view = m_pRenderingCamera->GetViewMatrix();
	buffer.inverseView = m_pRenderingCamera->GetViewInverseMatrix();
	buffer.projection = m_pRenderingCamera->GetProjectionMatrix();
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));
	buffer.worldViewProj = worldViewProj;

	pMaterial->UpdateMatrix(buffer);
}
void Renderer::UpdateLightMatrices(Light* pLight) const 
{
	MatrixTransformationContainer buffer;
	buffer.MarkAllForUpdate();

	// TO-DO: Based on the light type offset this
	Transform* pLightTransform = pLight->GetObject()->GetTransform();
	const DirectX::XMVECTOR& lightBackwards = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&pLightTransform->GetForward()), -50.0f);
	DirectX::XMFLOAT3 lightBackwardsFloat;
	DirectX::XMStoreFloat3(&lightBackwardsFloat, lightBackwards);
	DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixTranslation(lightBackwardsFloat.x, lightBackwardsFloat.y, lightBackwardsFloat.z));

	buffer.view = pLight->GetShadowViewMatrix();
	buffer.inverseView = pLight->GetShadowViewInverseMatrix();
	buffer.projection = pLight->GetShadowProjectionMatrix();
	DirectX::XMFLOAT4X4 worldViewProj;
	DirectX::XMStoreFloat4x4(&worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));
	buffer.worldViewProj = worldViewProj;

	m_pShadowCasting->GetShadowMaterial()->UpdateMatrix(buffer);
}
void Renderer::UpdateWorldMatrix(Component* pRenderables) const 
{
	MatrixTransformationContainer buffer;

	buffer.world = pRenderables->GetObject()->GetTransform()->GetWorld();
	buffer.updateWorld = true;

	m_pShadowCasting->GetShadowMaterial()->UpdateMatrix(buffer);
}
void Renderer::UpdateLights(Material* pMaterial) const
{
//	const int maxNrLights = Light::GetMaxNrLights();
//	std::vector<ShaderLight> lightsStructure{};
//	lightsStructure.reserve(maxNrLights);
//	for (int i{}; i < maxNrLights; ++i)
//	{
//		if (m_Lights.size() > i && m_Lights[i]->IsVisible())
//		{
//			//Add the light to structure for easy use
//			Transform* pTransform = m_Lights[i]->GetObject()->GetTransform();
//			DirectX::XMFLOAT3 pos = pTransform->GetPosition();
//			DirectX::XMFLOAT3 dir = pTransform->GetForward();
//			ShaderLight light{ 
//				DirectX::XMFLOAT4{pos.x, pos.y, pos.z, 1.0f},
//				DirectX::XMFLOAT4{dir.x, dir.y, dir.z, 1.0f},
//				m_Lights[i]->GetColor(),
//				m_Lights[i]->GetIntensity(),
//				m_Lights[i]->GetLightType(),
//				m_Lights[i]->GetShadowStartingIndex() };
//			lightsStructure.push_back(light);
//		}
//		else
//		{
//			ShaderLight light{};
//			lightsStructure.push_back(light);
//		}
//	}
//	pMaterial->UpdateMaterialLighting(m_pDevice->GetDeviceContext(), lightsStructure);
}
//void Renderer::UpdateShadows(Material* pMaterial, const Light* pLight) const
//{
//	pMaterial.
//}
//void Renderer::UpdateShadows(Material* pMaterial, const std::vector<Light*> pLights) const
//{
//}

void Renderer::RenderShadowDepths() const
{
	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShaderLight> lightsStructure{};
	lightsStructure.reserve(maxNrLights);

	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();

	// Loop over all lights and determine if they should be casting
	for (int i = 0; i < maxNrLights; ++i)
	{
		if (m_Lights.size() <= i || !m_Lights[i]->GetIsShadowCasting())
		{
			continue;
		}

		// If they can cast we push their info to the GPU for rendering
		ShadowData shadowData = m_Lights[i]->GetShadowData();
		UpdateLightMatrices(m_Lights[i]);
		// Set the shadow depthbuffer
		pContext->OMSetRenderTargets(0, nullptr, shadowData.m_pDepthStencil->GetStencilView());
		pContext->ClearDepthStencilView(shadowData.m_pDepthStencil->GetStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Run through all the renderables and render the shadow casting ones
		for (int i = 0; i < m_Renderables.size(); ++i)
		{
			if (m_Renderables[i]->IsShadowCasting())
			{
				// Update matrices
				UpdateWorldMatrix(m_Renderables[i]);
				// Render object to the buffer
				m_Renderables[i]->RenderShadow(m_pDevice->GetDevice(), pContext, m_pShadowCasting, 0);
			}
		}
	}
}

void Renderer::UpdateShadows(Material* pMaterial) const
{
}

void Renderer::ExplicitlyUnbindingRenderTargets(Material* pMaterial)
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11ShaderResourceView* srvs = nullptr;
	pContext->PSSetShaderResources(0, 1, &srvs);
	ID3D11RenderTargetView* nullRTV = nullptr; 
	pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	if (pMaterial != nullptr)
	{
		pMaterial->ExplicitlyUnbindingResources(m_pDevice);
	}
}
#pragma endregion
