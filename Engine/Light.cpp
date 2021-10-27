#include "pch.h"
#include "Light.h"

int Light::m_MaxLights = 10;

Light::Light()
	: m_Color{ DirectX::XMFLOAT3{1.0f,1.0f,1.0f} }
	, m_LightType{ LightType::directional }
	, m_Intensity{ 1.0f }
	, m_Enabled{ true }
{
	m_ShouldRender = true;
}
Light::~Light()
{
}
