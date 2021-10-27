#pragma once
#include "Component.h"
#include "ShaderStructures.h"

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
	inline DirectX::XMFLOAT3 GetColor() { return m_Color; }
	inline LightType GetLightType() { return m_LightType; }
	inline float GetIntensity() { return m_Intensity; }
	inline bool GetIsEnabled() { return m_Enabled; }

	inline static int GetMaxNrLights() { return m_MaxLights; }

protected:
	LightType m_LightType;

private:
	DirectX::XMFLOAT3 m_Color;
	float m_Intensity;
	bool m_Enabled;

	static int m_MaxLights;
};
