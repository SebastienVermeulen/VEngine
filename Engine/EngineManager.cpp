#include "pch.h"
#include "EngineManager.h"
#include "EngineDevice.h"

EngineDevice* EngineManager::m_pDevice = nullptr;
Renderer* EngineManager::m_pRenderer = nullptr;

EngineManager::EngineManager()
{
}
EngineManager::~EngineManager()
{
	SafeDelete(m_pDevice);
	SafeDelete(m_pRenderer);
}
