#pragma once
#include "Singleton.h"
#include "EngineDevice.h"
#include "Renderer.h"
#include "Window.h"
#include "WindowSettings.h"
#include "Project.h"

class EngineManager final : public Singleton<EngineManager>
{
public:
	EngineManager();
	virtual ~EngineManager();

	EngineManager(EngineManager& other) = delete;
	EngineManager(EngineManager&& other) = delete;
	EngineManager operator=(EngineManager& other) = delete;
	EngineManager& operator=(EngineManager&& other) = delete;

	inline EngineDevice* GetDevice(HWND windowHandle, const WindowSettings* settings)
	{
		if (!m_pDevice)
		{
			m_pDevice = new EngineDevice();
			m_pDevice->InitD3D(windowHandle, *settings);
		}
		return m_pDevice;
	}
	inline EngineDevice* GetDevice() const
	{
		return m_pDevice;
	}
	inline Renderer* GetRenderer(EngineDevice* device)
	{
		if (!m_pRenderer)
		{
			m_pRenderer = new Renderer(device);
		}
		return m_pRenderer;
	}
	inline Renderer* GetRenderer() const
	{
		return m_pRenderer;
	}
	inline Window* GetWindow(HINSTANCE hInstance, WindowSettings windowSettings, const int nCmdShow)
	{
		if (!m_pWindow)
		{
			m_pWindow = new Window(hInstance, windowSettings, nCmdShow);
		}
		return m_pWindow;
	}
	inline Window* GetWindow() const
	{
		return m_pWindow;
	}
	inline void SetProject(Project* pProject)
	{
		m_pOpenProject = pProject;
	}
	inline Project* GetProject() const
	{
		return m_pOpenProject;
	}

private:
	static EngineDevice* m_pDevice;
	static Renderer* m_pRenderer;
	static Window* m_pWindow;
	static Project* m_pOpenProject;
};
