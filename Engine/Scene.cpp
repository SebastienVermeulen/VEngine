#include "pch.h"
#include "Scene.h"
#include "Object.h"
#include "Camera.h"
#include "EngineManager.h"
#include "Renderer.h"
#include "AppTime.h"

Scene::Scene() 
	:BaseUpdateStructure()
	, m_Objects{}
{
	Init();
}
Scene::~Scene() 
{
	SafeDelete(m_Objects);
}

void Scene::Update(const float deltaTime)
{
	for (int i = 0; i < m_Objects.size(); ++i) 
	{
		m_Objects[i]->Update(deltaTime);
	}
}
void Scene::FixedUpdate() 
{
	for (int i = 0; i < m_Objects.size(); ++i)
	{
		m_Objects[i]->FixedUpdate();
	}
}
void Scene::LateUpdate(const float deltaTime)
{
	for (int i = 0; i < m_Objects.size(); ++i)
	{
		m_Objects[i]->LateUpdate(deltaTime);
	}
}

Object* Scene::CreateObject(std::string& name)
{
	Object* pTemp = new Object(name);
	m_Objects.push_back(pTemp);
	if (!pTemp->HasInit())
	{
		pTemp->Init();
		pTemp->MarkAsInit();
	}
	return pTemp;
}
