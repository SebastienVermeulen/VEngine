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
