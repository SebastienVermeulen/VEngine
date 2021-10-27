#include "pch.h"
#include "EngineManager.h"
#include "EngineDevice.h"

EngineDevice* EngineManager::m_pDevice = nullptr;
Renderer* EngineManager::m_pRenderer = nullptr;
Window* EngineManager::m_pWindow = nullptr;
Project* EngineManager::m_pOpenProject = nullptr;

EngineManager::EngineManager()
{
}
EngineManager::~EngineManager()
{
	SafeDelete(m_pDevice);
	SafeDelete(m_pRenderer);
	SafeDelete(m_pWindow);
}
