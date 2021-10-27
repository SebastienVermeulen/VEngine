#pragma once
#include "Widget.h"

class Light;

class LightWidget : public Widget
{
public:
	LightWidget(Light* pLight);
	virtual ~LightWidget();

	LightWidget(LightWidget& other) = delete;
	LightWidget(LightWidget&& other) = delete;
	LightWidget operator=(LightWidget& other) = delete;
	LightWidget& operator=(LightWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	Light* m_pLight;
};