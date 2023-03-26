#pragma once
#include "BaseUpdateStructure.h"
#include "WindowSettings.h"

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
	HRESULT Cleanup();

	Project* m_pProject;
	size_t m_MaxMessagesPerTick;
	bool m_Shutdown;
};
