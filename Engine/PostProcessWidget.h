#pragma once
#include "Widget.h"

class GreyScalePostProcess;

class GreyScalePostProcessWidget : public Widget
{
public:
	GreyScalePostProcessWidget(GreyScalePostProcess* pGreyScalePostProcess);
	virtual ~GreyScalePostProcessWidget();

	GreyScalePostProcessWidget(GreyScalePostProcessWidget& other) = delete;
	GreyScalePostProcessWidget(GreyScalePostProcessWidget&& other) = delete;
	GreyScalePostProcessWidget operator=(GreyScalePostProcessWidget& other) = delete;
	GreyScalePostProcessWidget& operator=(GreyScalePostProcessWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	GreyScalePostProcess* m_pGreyScalePostProcess;
};
