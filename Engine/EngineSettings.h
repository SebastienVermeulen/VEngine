#pragma once
#include "Singleton.h"
#include "WindowSettings.h"

class EngineManager;
enum class RenderType : unsigned int;

class EngineSettings final : public Singleton<EngineSettings>
{
public:
	EngineSettings();
	virtual ~EngineSettings();

	EngineSettings(EngineSettings& other) = delete;
	EngineSettings(EngineSettings&& other) = delete;
	EngineSettings operator=(EngineSettings& other) = delete;
	EngineSettings& operator=(EngineSettings&& other) = delete;

	// Should preferably only be used for initialization
	void SetRendertypeClean(RenderType renderType);
	void SetRendertype(RenderType renderType);
	RenderType GetRenderType() const;

	void MarkAllLightingDirty(EngineManager* pEngineManager);
	void MarkCurrentLightingDirty(EngineManager* pEngineManager);

	void SetVSync(bool FPSLimit);
	bool GetIfVSync() const;

	WindowSettings& GetWindowSettings();

private:
	WindowSettings m_WindowSettings;
	RenderType m_CurrentRenderType;
	//TO-DO: make Vsync not scummy
	bool m_VSync;
};