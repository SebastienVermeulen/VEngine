#pragma once
#include "Singleton.h"

class Object;

class UIRenderer final : public Singleton<UIRenderer>
{
public:
	UIRenderer();
	virtual ~UIRenderer();
	
	UIRenderer(UIRenderer& other) = delete;
	UIRenderer(UIRenderer&& other) = delete;
	UIRenderer operator=(UIRenderer& other) = delete;
	UIRenderer& operator=(UIRenderer&& other) = delete;

	void RenderUI();

private:
	void RenderSceneTree();
	void RenderObjectDetails();

	Object* m_SelectedObject;
	int m_SelectedObjectIdx;

	ImVec2 m_DetailsSize;
	ImVec2 m_DetailsPos;
};
