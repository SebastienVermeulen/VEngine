#include "pch.h"
#include "Object.h"
#include "Transform.h"
#include "ObjectWidget.h"

Object::Object(std::string& name) 
	:BaseUpdateStructure()
	, ComponentStructure()
	, m_pObjectWidget{ new ObjectWidget(this, name) }
	, m_pTransform{ new Transform()}
{
	Init();
}
Object::~Object() 
{
	SafeDelete(m_pTransform);
	SafeDelete(m_pObjectWidget);
}

bool Object::Init()
{
	return true;
}

void Object::Update(const float deltaTime)
{
	for (int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->Update(deltaTime);
	}
}
void Object::FixedUpdate()
{
	for (int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->FixedUpdate();
	}
}
void Object::LateUpdate(const float deltaTime)
{
	for (int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->LateUpdate(deltaTime);
	}
}
