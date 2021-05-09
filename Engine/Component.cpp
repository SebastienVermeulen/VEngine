#include "pch.h"
#include "Component.h"

Component::Component()
	:BaseUpdateStructure()
	, Renderable()
{
	m_ShouldRender = false;
}
Component::~Component() 
{
}
