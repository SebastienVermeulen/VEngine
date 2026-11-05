#include "pch.h"
#include "ShadowManager.h"
#include "Light.h"

ShadowManager::ShadowManager()
	:m_DirectionalShadowCastingLights{}
	, m_ShadowCastingLights{}
{
}
ShadowManager::~ShadowManager()
{
	m_DirectionalShadowCastingLights.clear();
	m_ShadowCastingLights.clear();
}

std::vector<Light*>& ShadowManager::GetDirectionalShadowCastingLights()
{
	return m_DirectionalShadowCastingLights;
}
std::vector<Light*>& ShadowManager::GetShadowCastingLights()
{
	return m_ShadowCastingLights;
}

void ShadowManager::TryAddLight(Light* pLight) 
{
	if (pLight->GetLightType() == LightType::directional)
	{
		if (std::find(m_DirectionalShadowCastingLights.begin(), m_DirectionalShadowCastingLights.end(), pLight) == m_DirectionalShadowCastingLights.end())
		{
			m_DirectionalShadowCastingLights.push_back(pLight);
		}
	}
	else 
	{
		if (std::find(m_ShadowCastingLights.begin(), m_ShadowCastingLights.end(), pLight) == m_ShadowCastingLights.end())
		{
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
			return true;
		}

		return false;
	}
	else
	{
		if (m_ShadowCastingLights.size())
		{
			return false;
		}

		std::vector<Light*>::iterator it = std::find(m_ShadowCastingLights.begin(), m_ShadowCastingLights.end(), pLight);
		if (it != m_ShadowCastingLights.end())
		{
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
