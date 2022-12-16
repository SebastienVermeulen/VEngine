#pragma once
#include "BaseUpdateStructure.h"
#include "WindowSettings.h"

class Window;
class Renderer;
class Project;

class App final : public BaseUpdateStructure
{
public:
	App(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings);
	~App();

	App(App& other) = delete;
	App(App&& other) = delete;
	App operator=(App& other) = delete;
	App& operator=(App&& other) = delete;

	int Run();
	void ResolveMessages(MSG& msg);

	void OpenProject(Project* project);

	inline void SetRendertype(RenderType renderType)
	{
		m_RenderType = renderType;
		//Make sure the other render type has up to date lighting info
		for (Renderer* renderer : EngineManager::Instance()->GetAllRenderer())
		{
			renderer->SetShouldUpdateLighting(true);
		}
	}
	inline RenderType GetRenderType() const
	{
		return m_RenderType;
	}
	inline void SetVSync(bool FPSLimit)
	{
		m_VSync = FPSLimit;
	}
	inline bool GetIfVSync() const
	{
		return m_VSync;
	}

private:
	HRESULT Init(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings);

	Project* m_pProject;

	RenderType m_RenderType;

	size_t m_MaxMessagesPerTick;
	//TO-DO: make Vsync not scummy
	bool m_VSync;
	bool m_Shutdown;
};
