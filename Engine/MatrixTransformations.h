#pragma once
#include "pch.h"
#include "Transform.h"
#include "Camera.h"

struct TransformationMatrixHelper
{
	static DirectX::XMFLOAT4X4 GetProjectionMatrix(bool perspectiveProjection, WindowSettings& windowSettings, CameraSettings& cameraSettings)
	{
		DirectX::XMFLOAT4X4 projectionMatrix;

		if (perspectiveProjection)
		{
			DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(cameraSettings.FOV, windowSettings.aspectRatio, cameraSettings.nearPlane, cameraSettings.farPlane));
		}
		else
		{
			const float viewWidth = (cameraSettings.size > 0) ? cameraSettings.size * windowSettings.aspectRatio : windowSettings.windowWidth;
			const float viewHeight = (cameraSettings.size > 0) ? cameraSettings.size : windowSettings.windowHeight;
			DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, cameraSettings.nearPlane, cameraSettings.farPlane));
		}

		return projectionMatrix;
	}
	static DirectX::XMFLOAT4X4 GetViewMatrix(const Transform* pTransform)
	{
		DirectX::XMFLOAT4X4 viewMatrix;

		const DirectX::XMVECTOR worldPosition = DirectX::XMLoadFloat3(&pTransform->GetWorldPosition());
		const DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&pTransform->GetForward());
		const DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&pTransform->GetUp());

		const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, DirectX::XMVectorAdd(worldPosition, lookAt), upVec);
		DirectX::XMStoreFloat4x4(&viewMatrix, view);

		return viewMatrix;
	}
	static DirectX::XMFLOAT4X4 GetInverseViewMatrix(const Transform* pTransform)
	{
		DirectX::XMFLOAT4X4 inverseViewMatrix;

		DirectX::XMFLOAT4X4 viewMatrix = GetViewMatrix(pTransform);
		const DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&viewMatrix));
		DirectX::XMStoreFloat4x4(&inverseViewMatrix, viewInv);

		return inverseViewMatrix;
	}
};