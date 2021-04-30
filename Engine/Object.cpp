#include "pch.h"
#include "Object.h"

Object::Object() 
	:BaseUpdateStructure()
	, ObjectStructure(this)
	, ComponentStructure()
{
}
Object::~Object() 
{
}

void Object::Init()
{
}
void Object::Cleanup()
{
}

void Object::Update(const float deltaTime)
{
}
void Object::FixedUpdate()
{
}
void Object::LateUpdate(const float deltaTime)
{
}
void Object::Render() const
{
}
