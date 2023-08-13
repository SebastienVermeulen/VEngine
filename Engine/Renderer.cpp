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

EngineDevice* Renderer::m_pDevice = nullptr;
EngineSettings* Renderer::m_pEngineSettings = nullptr;

Renderer::Renderer(EngineDevice* pDevice)
	:m_pRendererWidget{new RendererWidget()}
	, m_Renderables {}
	, m_Lights{}
	, m_pCameraList{}
	, m_pRenderingCamera{}
	, m_DefaultClearColor{ DirectX::XMFLOAT4{0.0f, 0.2f, 0.4f, 0.0f} }
	, m_NrBasisRenderTargets{6}
	, m_UpdateLighting{false}
{
    m_pDevice = pDevice;
	m_pEngineSettings = EngineSettings::Instance();

	m_Lights.reserve(Light::GetMaxNrLights());
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
	MatrixRenderBuffer buffer;

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
	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShaderLight> lightsStructure{};
	lightsStructure.reserve(maxNrLights);
	for (int i{}; i < maxNrLights; ++i)
	{
		if (m_Lights.size() > i && m_Lights[i]->IsVisible())
		{
			//Add the light to structure for easy use
			Transform* pTransform = m_Lights[i]->GetObject()->GetTransform();
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

void Renderer::ExplicitlyUnbindingRenderTargets() const
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11ShaderResourceView* srvs = nullptr;
	pContext->PSSetShaderResources(0, 1, &srvs);
	ID3D11RenderTargetView* nullRTV = nullptr; 
	pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
}
#pragma endregion
