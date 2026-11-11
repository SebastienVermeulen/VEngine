#include "pch.h"
#include "ShadowManager.h"
#include "Light.h"
#include "Material.h"
#include "ShadowCasting.h"
#include "Object.h"
#include "Transform.h"

ShadowManager::ShadowManager()
	:m_pShadowCasting{ new ShadowCasting() }
	, m_DirectionalShadowCastingLights{}
	, m_ShadowCastingLights{}
{
}
ShadowManager::~ShadowManager()
{
	m_DirectionalShadowCastingLights.clear();
	m_ShadowCastingLights.clear();

	SafeDelete(m_pShadowCasting);
}

void ShadowManager::InitMaterials(EngineDevice* pEngineDevice)
{
	m_pShadowCasting->GetShadowMaterial()->InitShader(pEngineDevice->GetDevice(), pEngineDevice->GetDeviceContext());
}

void ShadowManager::RenderShadowDepths(EngineDevice* pEngineDevice, const std::vector<Component*>& renderables) const
{
	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShaderLight> lightsStructure{};
	lightsStructure.reserve(maxNrLights);

	ID3D11Device* pDevice = pEngineDevice->GetDevice();
	ID3D11DeviceContext* pContext = pEngineDevice->GetDeviceContext();

	// Loop over all lights and determine if they should be casting
	for (Light* pLight : m_ShadowCastingLights)
	{
		if (!pLight->GetIsShadowCasting())
		{
			continue;
		}

		// If they can cast we push their info to the GPU for rendering
		ShadowData shadowData = pLight->GetShadowData();
		// We need to render 6 faces for pointlights, hence first check if a pointlight
		if (pLight->GetLightType() == LightType::point)
		{
			for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
			{
				UpdateLightMatrices(pLight, faceIndex);
				// Set the shadow depthbuffer
				pContext->OMSetRenderTargets(0, nullptr, shadowData.m_DepthStencils[faceIndex]->GetStencilView());

				// Run through all the renderables and render the shadow casting ones
				for (int i = 0; i < renderables.size(); ++i)
				{
					if (renderables[i]->IsShadowCasting())
					{
						// Update matrices
						UpdateWorldMatrix(renderables[i]);
						// Render object to the buffer
						renderables[i]->RenderShadow(pDevice, pContext, m_pShadowCasting, 0);
					}
				}
			}
		}
		else
		{
			UpdateLightMatrices(pLight, 0);
			// Set the shadow depthbuffer
			pContext->OMSetRenderTargets(0, nullptr, shadowData.m_DepthStencils[0]->GetStencilView());

			// Run through all the renderables and render the shadow casting ones
			for (int i = 0; i < renderables.size(); ++i)
			{
				if (renderables[i]->IsShadowCasting())
				{
					// Update matrices
					UpdateWorldMatrix(renderables[i]);
					// Render object to the buffer
					renderables[i]->RenderShadow(pDevice, pContext, m_pShadowCasting, 0);
				}
			}
		}
	}
}

void ShadowManager::UpdateShadowDepthData(EngineDevice* pEngineDevice) const
{
	for (Light* pDirLight : m_DirectionalShadowCastingLights)
	{
		TryGetShadowMaps(pEngineDevice->GetDeviceContext(), pDirLight);
	}
	for (Light* pLocalLight : m_ShadowCastingLights)
	{
		TryGetShadowMaps(pEngineDevice->GetDeviceContext(), pLocalLight);
	}
}
// TODO: Add permutation that allows 1 shadowmap to be uploaded
void ShadowManager::UpdateShadowProjectionData(Material* pMaterial, EngineDevice* pEngineDevice, Light* pLight) const
{
	pMaterial->UpdateMaterialShadowMaps(pEngineDevice, 1, std::vector<ShadowData>{ pLight->GetShadowData() });

	const std::vector<DirectX::XMFLOAT4X4> WVP = { pLight->ConstructWVPContainer() };
	pMaterial->UpdateLightingWVPBuffer(pEngineDevice, 1, WVP);
}
void ShadowManager::UpdateShadowProjectionData(Material* pMaterial, EngineDevice* pEngineDevice, const std::vector<Light*> pLights) const
{
	std::vector<DirectX::XMFLOAT4X4> WVP;

	const int maxNrLights = Light::GetMaxNrLights();
	std::vector<ShadowData> lightsStructure{};
	lightsStructure.reserve(maxNrLights);
	int totalShadowMaps = 0;

	for (int i{}; i < maxNrLights; ++i)
	{
		if (pLights.size() > i && pLights[i]->IsVisible() && pLights[i]->GetIsShadowCasting())
		{
			//Add the light to structure for easy use
			lightsStructure.push_back(pLights[i]->GetShadowData());
			WVP.append_range(pLights[i]->ConstructWVPContainer());

			totalShadowMaps += pLights[i]->GetLightType() == LightType::point ? 6 : 1;
		}
	}
	pMaterial->UpdateMaterialShadowMaps(pEngineDevice, totalShadowMaps, lightsStructure);

	pMaterial->UpdateLightingWVPBuffer(pEngineDevice, totalShadowMaps, WVP);
}

void ShadowManager::UpdateLightMatrices(Light* pLight, const int faceIndex) const
{
	m_pShadowCasting->GetShadowMaterial()->UpdateMatrix(pLight->ConstructMatrixContainer(faceIndex));
}
void ShadowManager::UpdateWorldMatrix(Component* pRenderable) const
{
	MatrixTransformationContainer buffer;

	buffer.world = pRenderable->GetObject()->GetTransform()->GetWorld();
	buffer.updateWorld = true;

	m_pShadowCasting->GetShadowMaterial()->UpdateMatrix(buffer);
}

std::vector<Light*>& ShadowManager::GetDirectionalShadowCastingLights()
{
	return m_DirectionalShadowCastingLights;
}
std::vector<Light*>& ShadowManager::GetLocalShadowCastingLights()
{
	return m_LocalShadowCastingLights;
}

void ShadowManager::TryAddLight(Light* pLight) 
{
	if (pLight->GetLightType() == LightType::directional)
	{
		if (std::find(m_DirectionalShadowCastingLights.begin(), m_DirectionalShadowCastingLights.end(), pLight) == m_DirectionalShadowCastingLights.end())
		{
			m_DirectionalShadowCastingLights.push_back(pLight);
			m_ShadowCastingLights.push_back(pLight);
		}
	}
	else 
	{
		if (std::find(m_LocalShadowCastingLights.begin(), m_LocalShadowCastingLights.end(), pLight) == m_LocalShadowCastingLights.end())
		{
			m_LocalShadowCastingLights.push_back(pLight);
			m_ShadowCastingLights.push_back(pLight);
		}
	}
}
bool ShadowManager::TryRemoveLight(Light* pLight) 
{
	if (pLight->GetLightType() == LightType::directional)
	{
		if (m_DirectionalShadowCastingLights.size())
		{
			return false;
		}

		std::vector<Light*>::iterator it = std::find(m_DirectionalShadowCastingLights.begin(), m_DirectionalShadowCastingLights.end(), pLight);
		if (it != m_DirectionalShadowCastingLights.end())
		{
			m_DirectionalShadowCastingLights.erase(it);
			m_ShadowCastingLights.erase(it);
			return true;
		}

		return false;
	}
	else
	{
		if (m_LocalShadowCastingLights.size())
		{
			return false;
		}

		std::vector<Light*>::iterator it = std::find(m_LocalShadowCastingLights.begin(), m_LocalShadowCastingLights.end(), pLight);
		if (it != m_LocalShadowCastingLights.end())
		{
			m_LocalShadowCastingLights.erase(it);
			m_ShadowCastingLights.erase(it);
			return true;
		}

		return false;
	}
}

void ShadowManager::TryGetShadowMaps(ID3D11DeviceContext* pContext, Light* pLight) const
{
	if (pLight->IsVisible() && pLight->GetIsShadowCasting())
	{
		EngineDevice* pEngineDevice = EngineManager::Instance()->GetDevice();

		// Update the number of assigned maps, only if they don't match
		// TODO: Define precompiler based numbers
		ShadowData shadowData = pLight->GetShadowData();
		if (!shadowData.IsEmpty())
		{
			if (pLight->GetLightType() == LightType::point && shadowData.m_DepthStencils.size() == 6)
			{
				return;
			}
			else if (shadowData.m_DepthStencils.size() == 1)
			{
				return;
			}
		}

		// TODO: Allow the light to determine the resolution
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = 1024;                 // All slices must share the same width
		desc.Height = 1024;                // All slices must share the same height
		desc.MipLevels = 1;                // Set > 1 if you have mipmaps
		desc.ArraySize = 1;                // Min
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (pLight->GetLightType() == LightType::point)
		{
			for (int i = 0; i < 6; ++i)
			{
				DepthStencil* pDepthStencil = pEngineDevice->TryGetDepthStencil(true, desc);
				// Clear the depth buffer to the max value
				pContext->ClearDepthStencilView(pDepthStencil->GetStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0);
				shadowData.Add(pDepthStencil);
			}
		}
		else
		{
			shadowData.Add(pEngineDevice->TryGetDepthStencil(true, desc));
		}

		pLight->SetShadowData(shadowData);
	}
}
