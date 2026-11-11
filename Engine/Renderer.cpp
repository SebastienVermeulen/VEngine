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
	, m_pShadowManager{ShadowManager::Instance()}
	, m_pPostProcessingPipeline{new PostProcessPipeline()}
	, m_DefaultClearColor{ DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 0.0f} }
	, m_NrBasisRenderTargets{5} // TO-DO: Uh oh, what was I thinking
	, m_UpdateLighting{false}
{
    m_pDevice = pDevice;
	m_pEngineSettings = EngineSettings::Instance();
}
Renderer::~Renderer()
{
	ShadowManager::ReleaseInstance();
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

	// Make sure our shadowCasting material also gets initialized
	m_pShadowManager->InitMaterials(m_pDevice);

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
	MarkTargetsAsNotUsed();

	m_RenderTargets.m_FinalTarget = nullptr;
	m_RenderTargets.m_FinalSceneTarget = nullptr;
	m_RenderTargets.m_DepthBuffer = nullptr;
}
void Renderer::MarkTargetsAsNotUsed()
{
	m_RenderTargets.m_FinalTarget->MarkAsUnused();
	m_RenderTargets.m_FinalSceneTarget->MarkAsUnused();
	m_RenderTargets.m_DepthBuffer->MarkAsUnused();
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
		if (pLight->IsShadowCasting())
		{
			m_pShadowManager->TryAddLight(pLight);
		}

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

		// We don't assume the light is not part of the shadow manager
		// The IsShadowCasting bool might be erroniously flipped
		// We want no dangling pointers left behind
		if (m_pShadowManager->TryRemoveLight(pLight))
		{
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
	buffer.updateInvWorldViewProj = false;

	buffer.world = pRenderable->GetObject()->GetTransform()->GetWorld();
	buffer.view = m_pRenderingCamera->GetViewMatrix();
	buffer.inverseView = m_pRenderingCamera->GetViewInverseMatrix();
	buffer.projection = m_pRenderingCamera->GetProjectionMatrix();
	DirectX::XMMATRIX worldViewProj = DirectX::XMLoadFloat4x4(&buffer.world)* DirectX::XMLoadFloat4x4(&buffer.view)* DirectX::XMLoadFloat4x4(&buffer.projection);
	DirectX::XMStoreFloat4x4(&buffer.worldViewProj, worldViewProj);

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

	DirectX::XMMATRIX worldViewProj = DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection);
	DirectX::XMStoreFloat4x4(&buffer.worldViewProj, worldViewProj);
	DirectX::XMStoreFloat4x4(&buffer.invWorldViewProj, DirectX::XMMatrixInverse(nullptr, worldViewProj));

	pMaterial->UpdateMatrix(buffer);
}
// TODO: Add array for non-casting lights to spare on byte usage
void Renderer::UpdateBatchedLights(Material* pMaterial) const
{
	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShaderLight> lightsStructure{};
	lightsStructure.reserve(maxNrLights);
	int shadowIndex = 0;

	for (int i{}; i < maxNrLights; ++i)
	{
		if (m_Lights.size() > i && m_Lights[i]->IsVisible())
		{
			// TODO: Handle non-shadowcasting
			lightsStructure.push_back(PopulateLightStruct(m_Lights[i], shadowIndex));

			m_Lights[i]->SetShadowStartingIndex(shadowIndex);
			shadowIndex += m_Lights[i]->GetLightType() == LightType::point ? 6 : 1;
		}
		else
		{
			// TODO: Why is this needed?
			ShaderLight light{};
			lightsStructure.push_back(light);
		}
	}
	pMaterial->UpdateMaterialLighting(m_pDevice->GetDeviceContext(), lightsStructure);
}
void Renderer::UpdateLight(Material* pMaterial, Light* pLight) const
{
	// We render 1 light at a time here, so the shadow index is always 0
	pMaterial->UpdateMaterialLighting(m_pDevice->GetDeviceContext(), { PopulateLightStruct(pLight, 0) });
}

ShaderLight Renderer::PopulateLightStruct(Light* pLight, const int shadowIndex) const
{
	//Add the light to structure for easy use
	Transform* pTransform = pLight->GetObject()->GetTransform();
	DirectX::XMFLOAT3 pos = pTransform->GetPosition();
	DirectX::XMFLOAT3 dir = pTransform->GetForward();
	ShaderLight light{
		DirectX::XMFLOAT4{pos.x, pos.y, pos.z, 1.0f},
		DirectX::XMFLOAT4{dir.x, dir.y, dir.z, 1.0f},
		pLight->GetColor(),
		pLight->GetIntensity(),
		pLight->GetLightType(),
		shadowIndex };

	return light;
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
