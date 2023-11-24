#include "pch.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "EngineSettings.h"

EngineDevice* EngineManager::m_pDevice = nullptr;
Window* EngineManager::m_pWindow = nullptr;
Project* EngineManager::m_pOpenProject = nullptr;

EngineManager::EngineManager()
{
}
EngineManager::~EngineManager()
{
	SafeDelete(m_pOpenProject);
	SafeDelete(m_pDevice);
	SafeDelete(m_pWindow);
}

void EngineManager::Render()
{
	GetActiveRenderer()->Render();
}

void EngineManager::UpdateTargetLifeTimes(const float deltaTime)
{
	m_pDevice->UpdateTargetLifeTimes(deltaTime);
}