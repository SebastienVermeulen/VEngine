#include "pch.h"
#include "Component.h"
#include "Object.h"
#include "Widget.h"

Component::Component()
	:BaseUpdateStructure()
	, Renderable()
	, m_pWidget{}
{
	m_ShouldRender = false;
}
Component::~Component() 
{
	SafeDelete(m_pWidget);
}
