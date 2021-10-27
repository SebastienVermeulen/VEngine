#pragma once
#include "Widget.h"

class MeshComponent;

class MeshWidget : public Widget
{
public:
	MeshWidget(MeshComponent* pMeshComponent);
	virtual ~MeshWidget();

	MeshWidget(MeshWidget& other) = delete;
	MeshWidget(MeshWidget&& other) = delete;
	MeshWidget operator=(MeshWidget& other) = delete;
	MeshWidget& operator=(MeshWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

private:
	MeshComponent* m_pMeshComponent;
};