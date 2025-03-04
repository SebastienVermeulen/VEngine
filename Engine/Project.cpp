#include "pch.h"
#include "Project.h"
#include "Scene.h"

Project::Project() 
	:BaseUpdateStructure()
	, m_Scenes{}
	, m_pCurrentScene{}
{
	Init();
}
Project::~Project()
{
	std::vector<Scene*>::iterator pScene =
		std::find(m_Scenes.begin(), m_Scenes.end(), m_pCurrentScene);
	m_Scenes.erase(pScene);

	SafeDelete(m_pCurrentScene);
	SafeDelete(m_Scenes);
}

bool Project::Init() 
{
	if (!m_pCurrentScene)
	{
		m_pCurrentScene = new Scene();
		m_Scenes.push_back(m_pCurrentScene);
		return m_pCurrentScene->Init();
	}
	return true;
}

void Project::Update(const float deltaTime) 
{
	m_pCurrentScene->Update(deltaTime);
}
void Project::FixedUpdate() 
{
	m_pCurrentScene->FixedUpdate();
}
void Project::LateUpdate(const float deltaTime)
{
	m_pCurrentScene->LateUpdate(deltaTime);
}
void Project::Render() const 
{
	m_pCurrentScene->Render();
}
