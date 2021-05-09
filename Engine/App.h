#pragma once
#include "BaseUpdateStructure.h"

class Window;
class Renderer;
class Project;

class App final : public BaseUpdateStructure
{
public:
	App(HINSTANCE hInstance, const int nCmdShow);
	~App();

	App(App& other) = delete;
	App(App&& other) = delete;
	App operator=(App& other) = delete;
	App& operator=(App&& other) = delete;

	int Run();

	void OpenProject(Project* project);

private:
	LRESULT Init(HINSTANCE hInstance, const int nCmdShow);

	Project* m_pProject;
	Window* m_pWindow;
	Renderer* m_pRenderer;
};
