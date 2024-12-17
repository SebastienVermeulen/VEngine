#pragma once
#include "Component.h"
#include "Window.h"
#include "WindowSettings.h"
#include "Camera.h"
#include "EngineManager.h"

struct CameraSettings final
{
public:
	static inline CameraSettings Default()
	{
		CameraSettings cameraSetting{ 25.0f, DirectX::XM_PIDIV4, 100.0f, 0.01f };
		return cameraSetting;
	}

	float size;
	float FOV;
	float farPlane;
	float nearPlane;
};

class Camera : public Component
{
public:
	Camera(CameraSettings settings);
	virtual ~Camera();

	virtual bool Init() override;

	Camera(Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera operator=(Camera& other) = delete;
	Camera& operator=(Camera&& other) = delete;

	DirectX::XMFLOAT4X4& GetProjectionMatrix();
	DirectX::XMFLOAT4X4& GetViewMatrix();
	DirectX::XMFLOAT4X4& GetViewInverseMatrix();

	inline void SetCameraSettings(CameraSettings settings) { m_Settings = settings; }
	inline CameraSettings& GetCameraSettings() { return m_Settings; }

private:
	DirectX::XMFLOAT4X4 m_ProjectionMatrix, m_ViewMatrix, m_InverseViewMatrix;

	CameraSettings m_Settings;

	bool m_PerspectiveProjection;
};