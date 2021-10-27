#pragma once
#include "Widget.h"

class Camera;

class CameraWidget : public Widget
{
public:
	CameraWidget(Camera* pCamera);
	virtual ~CameraWidget();

	CameraWidget(CameraWidget& other) = delete;
	CameraWidget(CameraWidget&& other) = delete;
	CameraWidget operator=(CameraWidget& other) = delete;
	CameraWidget& operator=(CameraWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	Camera* m_pCamera;
};