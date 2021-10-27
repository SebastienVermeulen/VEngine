#pragma once
#include "Widget.h"

class Transform;

class TransformWidget : public Widget
{
public:
	TransformWidget(Transform* pTransform);
	virtual ~TransformWidget();

	TransformWidget(TransformWidget& other) = delete;
	TransformWidget(TransformWidget&& other) = delete;
	TransformWidget operator=(TransformWidget& other) = delete;
	TransformWidget& operator=(TransformWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	Transform* m_pTransform;
};