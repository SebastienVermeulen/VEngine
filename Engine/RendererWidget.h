#pragma once
#include "Widget.h"

class Renderer;

class RendererWidget : public Widget
{
public:
	RendererWidget();
	virtual ~RendererWidget();

	RendererWidget(RendererWidget& other) = delete;
	RendererWidget(RendererWidget&& other) = delete;
	RendererWidget operator=(RendererWidget& other) = delete;
	RendererWidget& operator=(RendererWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;
};