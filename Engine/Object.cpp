#include "pch.h"
#include "Object.h"

Object::Object() 
	:BaseUpdateStructure()
	, ObjectStructure(this)
	, ComponentStructure()
{
	Init();
}
Object::~Object() 
{
}

void Object::Init()
{
}

void Object::Update(const float deltaTime)
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->Update(deltaTime);
	}
}
void Object::FixedUpdate()
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->FixedUpdate();
	}
}
void Object::LateUpdate(const float deltaTime)
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components[i]->LateUpdate(deltaTime);
	}
}
