#pragma once
#include "Singleton.h"
#include "EngineDevice.h"
#include "EngineSettings.h"
#include "Renderer.h"
#include "Window.h"
#include "WindowSettings.h"
#include "Project.h"

//TO-DO: Remove these dependencies, or at least move them to the cpp
#include "DeferredDX11.h"
#include "ForwardsDX11.h"

class EngineManager final : public Singleton<EngineManager>
{
public:
	EngineManager();
	virtual ~EngineManager();

	EngineManager(EngineManager& other) = delete;
	EngineManager(EngineManager&& other) = delete;
	EngineManager operator=(EngineManager& other) = delete;
	EngineManager& operator=(EngineManager&& other) = delete;

	void Render();

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
	inline Renderer* GetActiveRenderer()
	{
		if (m_pRenderers.empty())
		{
			CreateRenderers();
		}

		RenderType type = EngineSettings::Instance()->GetRenderType();
		for (Renderer* pRenderer : m_pRenderers)
		{
			if (pRenderer->GetRenderType() == type)
			{
				return pRenderer;
			}
		}
		return nullptr;
	}
	inline Renderer* GetRenderer(RenderType type)
	{
		if (m_pRenderers.empty())
		{
			CreateRenderers();
		}

		for (Renderer* pRenderer : m_pRenderers)
		{
			if (pRenderer->GetRenderType() == type)
			{
				return pRenderer;
			}
		}
		return nullptr;
	}
	inline std::vector<Renderer*> GetAllRenderers() const
	{
		return m_pRenderers;
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
	inline std::vector<RenderType>& GetAvailableRenderTypes() 
	{
		return m_pRenderTypes;
	}

private:
	//TO-DO: Make this create renderers based on users wishes, lets say the main uberlevel is deferredDX11 then only that renderer needs to be allocated
	inline void CreateRenderers() 
	{
		AddRenderer(new DeferredDX11(m_pDevice));
		AddRenderer(new ForwardsDX11(m_pDevice));
	}
	inline void AddRenderer(Renderer* pRenderer) 
	{
		m_pRenderers.push_back(pRenderer);
		m_pRenderTypes.push_back(pRenderer->GetRenderType());
	}

private:
	static EngineDevice* m_pDevice;
	std::vector<Renderer*> m_pRenderers;
	std::vector<RenderType> m_pRenderTypes;
	static Window* m_pWindow;
	static Project* m_pOpenProject;
};
