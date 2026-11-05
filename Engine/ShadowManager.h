#pragma once
#include "Singleton.h"

class Light;

class ShadowManager : public Singleton<ShadowManager>
{
public:
	ShadowManager();
	virtual ~ShadowManager();

	ShadowManager(ShadowManager& other) = delete;
	ShadowManager(ShadowManager&& other) = delete;
	ShadowManager operator=(ShadowManager& other) = delete;
	ShadowManager& operator=(ShadowManager&& other) = delete;

	std::vector<Light*>& GetDirectionalShadowCastingLights();
	std::vector<Light*>& GetShadowCastingLights();

	void TryAddLight(Light* pLight);
	bool TryRemoveLight(Light* pLight);

	void TryGetShadowMaps(ID3D11DeviceContext* pContext, Light* pLight) const;

private:
	std::vector<Light*> m_DirectionalShadowCastingLights;
	std::vector<Light*> m_ShadowCastingLights;
};
