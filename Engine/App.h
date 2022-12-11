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

private:
	HRESULT Init(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings);

	Project* m_pProject;
	Window* m_pWindow;
	Renderer* m_pRenderer;

	size_t m_MaxMessagesPerTick;
	bool m_Shutdown;
};
