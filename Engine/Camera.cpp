#include "pch.h"
#include "Camera.h"
#include "Object.h"
#include "CameraWidget.h"
#include "Transform.h"
#include "MatrixTransformations.h"

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

	m_ProjectionMatrix = TransformationMatrixHelper::GetProjectionMatrix(m_PerspectiveProjection, windowSettings, m_Settings);

	return m_ProjectionMatrix;
}
DirectX::XMFLOAT4X4& Camera::GetViewMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	m_ViewMatrix = TransformationMatrixHelper::GetViewMatrix(pTransform);

	return m_ViewMatrix;
}
DirectX::XMFLOAT4X4& Camera::GetViewInverseMatrix()
{
	Transform* pTransform = GetObject()->GetTransform();

	m_InverseViewMatrix = TransformationMatrixHelper::GetInverseViewMatrix(pTransform);

	return m_InverseViewMatrix;
}
