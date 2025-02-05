#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "MatrixTransformations.h"
#include "Object.h"

int Light::m_MaxLights = 10;

Light::Light()
	: m_Color{ DirectX::XMFLOAT3{1.0f,1.0f,1.0f} }
	, m_LightType{ LightType::directional }
	, m_Intensity{ 1.0f }
	, m_Enabled{ true }
{
	m_CanRender = true;
}
Light::~Light()
{
}

void Light::SetIsShadowCasting(bool enabled)
{
	m_ShadowCasting = enabled; 
	if (!m_ShadowCasting)
	{
		m_ShadowData.m_pDepthStencil = nullptr;
	}
	else 
	{
		// TO-DO: Run this from a manager
		m_ShadowData.m_pDepthStencil = EngineManager::Instance()->GetDevice()->TryGetDepthStencil(false);
	}
}

DirectX::XMFLOAT4X4& Light::GetShadowProjectionMatrix()
{
	WindowSettings windowSettings = WindowSettings{ 1, 1 };
	// TO-DO: add spotlights
	CameraSettings cameraSettings = CameraSettings{ -1, 0, 1000.0f, 0.01f };

	m_ProjectionMatrix = TransformationMatrixHelper::GetProjectionMatrix(m_LightType != LightType::directional, windowSettings, cameraSettings);

	return m_ProjectionMatrix;
}
DirectX::XMFLOAT4X4& Light::GetShadowViewMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	m_ViewMatrix = TransformationMatrixHelper::GetViewMatrix(pTransform);

	return m_ViewMatrix;
}
DirectX::XMFLOAT4X4& Light::GetShadowViewInverseMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	m_InverseViewMatrix = TransformationMatrixHelper::GetInverseViewMatrix(pTransform);

	return m_InverseViewMatrix;
}
