#pragma once
#include "Widget.h"

class Object;

class ObjectWidget : public Widget
{
public:
	ObjectWidget(Object* pOwningObject, std::string& name);
	virtual ~ObjectWidget();

	ObjectWidget(ObjectWidget& other) = delete;
	ObjectWidget(ObjectWidget&& other) = delete;
	ObjectWidget operator=(ObjectWidget& other) = delete;
	ObjectWidget& operator=(ObjectWidget&& other) = delete;

	virtual bool RenderUITitle(int idx) override;
	virtual void RenderUIElement(int idx) override;

	inline bool WasSelected() const 
	{
		return m_WasSelected;
	}
	inline void SetWasSelected(bool wasSelected) 
	{
		m_WasSelected = wasSelected;
	}
	inline std::string GetTitle() const 
	{
		return m_Title;
	}

private:
	Object* m_pOwningObject;
	std::string m_Title;
	bool m_WasSelected;
};