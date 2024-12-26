#include "pch.h"
#include "ShadowManager.h"

ShadowManager::ShadowManager()
	:m_DirectionalShadowCastingLights{}
	, m_ShadowCastingLights{}
{
}
ShadowManager::~ShadowManager()
{
}

std::vector<Light*>& ShadowManager::GetDirectionalShadowCastingLights()
{
	return m_DirectionalShadowCastingLights;
}
std::vector<Light*>& ShadowManager::GetShadowCastingLights()
{
	return m_ShadowCastingLights;
}
