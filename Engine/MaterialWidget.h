#pragma once
#include "Widget.h"

class Material;

class MaterialWidget : public Widget
{
public:
	MaterialWidget(Material* pMaterial);
	virtual ~MaterialWidget();

	MaterialWidget(MaterialWidget& other) = delete;
	MaterialWidget(MaterialWidget&& other) = delete;
	MaterialWidget operator=(MaterialWidget& other) = delete;
	MaterialWidget& operator=(MaterialWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	Material* m_pMaterial;
};