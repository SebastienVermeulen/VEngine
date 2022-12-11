#pragma once
#include "BaseUpdateStructure.h"
#include "Renderable.h"

class Object;
class Widget;

class Component abstract : public BaseUpdateStructure, public Renderable
{
public:
	Component();
	virtual ~Component();

	Component(Component& other) = delete;
	Component(Component&& other) = delete;
	Component operator=(Component& other) = delete;
	Component& operator=(Component&& other) = delete;

	inline Object* GetObject() const 
	{ 
		return m_pObject; 
	}
	void SetObject(Object* pObject) 
	{ 
		m_pObject = pObject; 
	}

	inline Widget* GetWidget() const 
	{
		return m_pWidget;
	}

protected:
	Widget* m_pWidget;

private:
	Object* m_pObject;
};