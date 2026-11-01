#pragma once

class Light;

class ShadowManager
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

private:
	std::vector<Light*> m_DirectionalShadowCastingLights;
	std::vector<Light*> m_ShadowCastingLights;
};
