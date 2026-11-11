#pragma once
#include "Singleton.h"

class Light;
class Material;
class ShadowCasting;
class EngineDevice;
class Component;

class ShadowManager : public Singleton<ShadowManager>
{
public:
	ShadowManager();
	virtual ~ShadowManager();

	ShadowManager(ShadowManager& other) = delete;
	ShadowManager(ShadowManager&& other) = delete;
	ShadowManager operator=(ShadowManager& other) = delete;
	ShadowManager& operator=(ShadowManager&& other) = delete;

	// Material Data Init Related
	void InitMaterials(EngineDevice* pEngineDevice);

	void RenderShadowDepths(EngineDevice* pEngineDevice, const std::vector<Component*>& renderables) const;

	void UpdateShadowDepthData(EngineDevice* pEngineDevice) const;
	void UpdateShadowProjectionData(Material* pMaterial, EngineDevice* pEngineDevice, Light* pLight) const;
	void UpdateShadowProjectionData(Material* pMaterial, EngineDevice* pEngineDevice, const std::vector<Light*> pLights) const;

	void UpdateLightMatrices(Light* pLight, const int faceIndex) const;
	void UpdateWorldMatrix(Component* pRenderables) const;

	// CPU Data Related
	std::vector<Light*>& GetDirectionalShadowCastingLights();
	std::vector<Light*>& GetLocalShadowCastingLights();

	void TryAddLight(Light* pLight);
	bool TryRemoveLight(Light* pLight);

	void TryGetShadowMaps(ID3D11DeviceContext* pContext, Light* pLight) const;

private:
	// ShadowDepth Material
	ShadowCasting* m_pShadowCasting;

	std::vector<Light*> m_ShadowCastingLights;
	std::vector<Light*> m_DirectionalShadowCastingLights;
	std::vector<Light*> m_LocalShadowCastingLights;
};
