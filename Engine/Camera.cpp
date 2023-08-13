#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Object.h"
#include "CameraWidget.h"

Camera::Camera(CameraSettings settings)
	:m_ProjectionMatrix{}
	, m_ViewMatrix{}
	, m_InverseViewMatrix{}
	, m_Settings {settings}
	, m_PerspectiveProjection{true}
{
	m_CanRender = true;

	m_pWidget = new CameraWidget(this);
}
Camera::~Camera() 
{
	SafeDelete(m_pWidget);
}

bool Camera::Init() 
{
	m_CanRender = true;
	return true;
}

DirectX::XMFLOAT4X4& Camera::GetProjectionMatrix()
{
	Window* pWindow = EngineManager::Instance()->GetWindow();
	WindowSettings windowSettings = pWindow->GetWindowSettings();

	if (m_PerspectiveProjection)
	{
		DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH(m_Settings.FOV, windowSettings.aspectRatio, m_Settings.nearPlane, m_Settings.farPlane));
	}
	else
	{
		const float viewWidth = (m_Settings.size > 0) ? m_Settings.size * windowSettings.aspectRatio : windowSettings.windowWidth;
		const float viewHeight = (m_Settings.size > 0) ? m_Settings.size : windowSettings.windowHeight;
		DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_Settings.nearPlane, m_Settings.farPlane));
	}

	return m_ProjectionMatrix;
}
DirectX::XMFLOAT4X4& Camera::GetViewMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	const DirectX::XMVECTOR worldPosition = DirectX::XMLoadFloat3(&pTransform->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&pTransform->GetForward());
	const DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&pTransform->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, DirectX::XMVectorAdd(worldPosition, lookAt), upVec);
	const DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, view);
	DirectX::XMStoreFloat4x4(&m_InverseViewMatrix, viewInv);

	const DirectX::XMFLOAT4X4 projection = GetProjectionMatrix();
	const DirectX::XMMATRIX viewProjectionInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixMultiply(view, DirectX::XMLoadFloat4x4(&projection)));
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, view);

	return m_ViewMatrix;
}
DirectX::XMFLOAT4X4& Camera::GetViewInverseMatrix()
{
	return m_InverseViewMatrix;
}
