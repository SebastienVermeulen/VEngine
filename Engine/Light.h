#pragma once
#include "Component.h"
#include "ShaderStructures.h"
#include "ShadowData.h"

class Light abstract : public Component
{
public:
	Light();
	virtual ~Light();

	Light(Light& other) = delete;
	Light(Light&& other) = delete;
	Light operator=(Light& other) = delete;
	Light& operator=(Light&& other) = delete;

	inline void SetColor(DirectX::XMFLOAT3 color) { m_Color = color; }
	inline void SetIntensity(float intensity) { m_Intensity = intensity; }
	inline void SetIsEnabled(bool enabled) { m_Enabled = enabled; }
	void SetIsShadowCasting(bool enabled);
	inline void SetShadowData(ShadowData shadowData) { m_ShadowData = shadowData; }

	inline DirectX::XMFLOAT3 GetColor() { return m_Color; }
	inline LightType GetLightType() { return m_LightType; }
	inline float GetIntensity() { return m_Intensity; }
	inline bool GetIsEnabled() { return m_Enabled; }
	inline bool GetIsShadowCasting() { return m_ShadowCasting; }
	inline ShadowData GetShadowData() { return m_ShadowData; }

	inline static int GetMaxNrLights() { return m_MaxLights; }

	DirectX::XMFLOAT4X4& GetShadowProjectionMatrix();
	DirectX::XMFLOAT4X4& GetShadowViewMatrix();
	DirectX::XMFLOAT4X4& GetShadowViewInverseMatrix();

protected:
	LightType m_LightType;

private:
	// Lighting
	DirectX::XMFLOAT3 m_Color;
	float m_Intensity;
	bool m_Enabled;

	// Shadows
	ShadowData m_ShadowData;
	DirectX::XMFLOAT4X4 m_ProjectionMatrix, m_ViewMatrix, m_InverseViewMatrix;
	bool m_ShadowCasting;

	// Statics
	static int m_MaxLights;
};
