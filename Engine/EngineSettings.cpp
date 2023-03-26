#include "pch.h"
#include "EngineSettings.h"
#include "Renderable.h"
#include "Renderer.h"
#include "EngineManager.h"

EngineSettings::EngineSettings()
	:m_WindowSettings{}
	, m_CurrentRenderType{}
	, m_VSync{}
{
}
EngineSettings::~EngineSettings()
{
}

void EngineSettings::SetRendertypeClean(RenderType renderType)
{
	//Make sure the other render type has up to date lighting info
	EngineManager* pEngineManager = EngineManager::Instance();
	m_CurrentRenderType = renderType;
}
void EngineSettings::SetRendertype(RenderType renderType)
{
	//Make sure the other render type has up to date lighting info
	EngineManager* pEngineManager = EngineManager::Instance();
	MarkCurrentLightingDirty(pEngineManager);
	m_CurrentRenderType = renderType;
	MarkCurrentLightingDirty(pEngineManager);
}
RenderType EngineSettings::GetRenderType() const
{
	return m_CurrentRenderType;
}

void EngineSettings::MarkAllLightingDirty(EngineManager* pEngineManager)
{
	for (Renderer* pRenderer : pEngineManager->GetAllRenderers())
	{
		pRenderer->SetShouldUpdateLighting(true);
	}
}
void EngineSettings::MarkCurrentLightingDirty(EngineManager* pEngineManager)
{
	pEngineManager->GetActiveRenderer()->SetShouldUpdateLighting(true);
}

void EngineSettings::SetVSync(bool FPSLimit)
{
	m_VSync = FPSLimit;
}
bool EngineSettings::GetIfVSync() const
{
	return m_VSync;
}
