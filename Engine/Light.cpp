#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "MatrixTransformations.h"
#include "Object.h"
#include "ShadowManager.h"

int Light::m_MaxLights = 10;

Light::Light()
	: m_Color{ DirectX::XMFLOAT3{1.0f,1.0f,1.0f} }
	, m_LightType{ LightType::directional }
	, m_Intensity{ 1.0f }
	, m_Enabled{ true }
	, m_NearPlane{ 0.01f }
	, m_FarPlane{ 100.0f }
	, m_ShadowCasting{ true }
	, m_ShadowStartingIndex{ 0 }
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
		m_ShadowData.Clear();
	}
}

DirectX::XMFLOAT4X4& Light::GetShadowProjectionMatrix()
{
	// TO-DO: Scale this somehow? 
	WindowSettings windowSettings = WindowSettings{ 1, 1, 1 };
	// TO-DO: add spotlights
	CameraSettings cameraSettings = CameraSettings{ -1, DirectX::XM_PIDIV2, 100.0f, 0.01f };

	m_ProjectionMatrix = TransformationMatrixHelper::GetProjectionMatrix(m_LightType != LightType::directional, windowSettings, cameraSettings);

	return m_ProjectionMatrix;
}
DirectX::XMFLOAT4X4& Light::GetShadowViewMatrix(const int viewIndex)
{
	Transform* pTransform = GetObject()->GetTransform();

	DirectX::XMVECTOR lightPos = DirectX::XMLoadFloat3(&pTransform->GetWorldPosition());

	if (m_LightType == LightType::point)
	{
		// Define where each of the 6 flat shadow cameras is looking (relative to light position)
		DirectX::XMVECTOR targets[6] = {
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(1.0f,  0.0f,  0.0f, 0.0f)),  // Face 0: Right (+X)
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(-1.0f,  0.0f,  0.0f, 0.0f)),  // Face 1: Left (-X)
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(0.0f,  1.0f,  0.0f, 0.0f)),  // Face 2: Up (+Y)
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(0.0f, -1.0f,  0.0f, 0.0f)),  // Face 3: Down (-Y)
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(0.0f,  0.0f,  1.0f, 0.0f)),  // Face 4: Forward (+Z)
			DirectX::XMVectorAdd(lightPos, DirectX::XMVectorSet(0.0f,  0.0f, -1.0f, 0.0f))   // Face 5: Backward (-Z)
		};

		// Define the Up vector for each independent 2D texture
		DirectX::XMVECTOR ups[6] = {
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),  // Right face -> World Up is texture Up
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),  // Left face -> World Up is texture Up
			DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),  // Up face -> World Forward (+Z) is texture Up
			DirectX::XMVectorSet(0.0f, 0.0f,-1.0f, 0.0f),  // Down face -> World Backward (-Z) is texture Up
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),  // Forward face -> World Up is texture Up
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)   // Backward face -> World Up is texture Up
		};

		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixLookAtLH(lightPos, targets[viewIndex], ups[viewIndex]));
	}
	else 
	{
		const DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&pTransform->GetForward());
		const DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&pTransform->GetUp());

		// Offset the directional light along its forward vector
		// This is a bit personal preference, but I find it logical to offset halfway along the origin
		// However, realistically it should instead be relative to the camera's position
		lightPos = DirectX::XMVectorSubtract(lightPos, DirectX::XMVectorScale(lookAt, 0.5f * (m_FarPlane - m_NearPlane)));

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(lightPos, DirectX::XMVectorAdd(lightPos, lookAt), upVec);
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, view);
	}

	return m_ViewMatrix;
}
DirectX::XMFLOAT4X4& Light::GetShadowViewInverseMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	m_InverseViewMatrix = TransformationMatrixHelper::GetInverseViewMatrix(pTransform);

	return m_InverseViewMatrix;
}

MatrixTransformationContainer Light::ConstructMatrixContainer(const int faceIndex)
{
	MatrixTransformationContainer buffer;
	buffer.MarkAllForUpdate();

	// TO-DO: Based on the light type offset this
	Transform* pLightTransform = GetObject()->GetTransform();
	DirectX::XMFLOAT3 pos = pLightTransform->GetPosition();

	if (m_LightType == LightType::point)
	{
		DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&buffer.view, DirectX::XMLoadFloat4x4(&GetShadowViewMatrix(faceIndex)));
		// "Cubemaps" require a strict 90-degree field of view with a 1:1 aspect ratio
		DirectX::XMStoreFloat4x4(&buffer.projection, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, m_FarPlane, m_NearPlane));
	}
	//else if (type == LightType::spot)
	//{
	//	shadowView = DirectX::XMMatrixLookAtLH(lightPos, DirectX::XMVectorAdd(lightPos, lightForward), lightUp);
	//	shadowProj = DirectX::XMLoadFloat4x4(&pLight->GetShadowProjectionMatrix());
	//}
	else // Directional Light
	{
		// TODO: Ideally calculated dynamically based on the main camera's frustum bounding box
		DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&buffer.view, DirectX::XMLoadFloat4x4(&GetShadowViewMatrix(-1)));
		DirectX::XMStoreFloat4x4(&buffer.projection, DirectX::XMLoadFloat4x4(&GetShadowProjectionMatrix()));
	}

	DirectX::XMStoreFloat4x4(&buffer.inverseView, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&buffer.view)));
	DirectX::XMStoreFloat4x4(&buffer.worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));

	return buffer;
}
std::vector<DirectX::XMFLOAT4X4> Light::ConstructWVPContainer()
{
	std::vector<DirectX::XMFLOAT4X4> WVP;

	MatrixTransformationContainer buffer;
	buffer.MarkAllForUpdate();

	// TO-DO: Based on the light type offset this
	Transform* pLightTransform = GetObject()->GetTransform();
	DirectX::XMFLOAT3 pos = pLightTransform->GetPosition();

	if (m_LightType == LightType::point)
	{
		for (int i = 0; i < 6; ++i)
		{
			DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
			DirectX::XMStoreFloat4x4(&buffer.view, DirectX::XMLoadFloat4x4(&GetShadowViewMatrix(i)));
			DirectX::XMStoreFloat4x4(&buffer.projection, DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, m_FarPlane, m_NearPlane));
			DirectX::XMStoreFloat4x4(&buffer.worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));

			WVP.push_back(buffer.worldViewProj);
		}

		return WVP;
	}
	//else if (type == LightType::spot)
	//{
	//	shadowView = DirectX::XMMatrixLookAtLH(lightPos, DirectX::XMVectorAdd(lightPos, lightForward), lightUp);
	//	shadowProj = DirectX::XMLoadFloat4x4(&pLight->GetShadowProjectionMatrix());
	//}
	else // Directional Light
	{
		// TODO: Ideally calculated dynamically based on the main camera's frustum bounding box
		DirectX::XMStoreFloat4x4(&buffer.world, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&buffer.view, DirectX::XMLoadFloat4x4(&GetShadowViewMatrix(-1)));
		DirectX::XMStoreFloat4x4(&buffer.projection, DirectX::XMLoadFloat4x4(&GetShadowProjectionMatrix()));
		DirectX::XMStoreFloat4x4(&buffer.worldViewProj, DirectX::XMLoadFloat4x4(&buffer.world) * DirectX::XMLoadFloat4x4(&buffer.view) * DirectX::XMLoadFloat4x4(&buffer.projection));

		WVP.push_back(buffer.worldViewProj);
		return WVP;
	}
}
