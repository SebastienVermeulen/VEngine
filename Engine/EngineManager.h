#pragma once
#include "Singleton.h"
#include "EngineDevice.h"
#include "Renderer.h"
#include "WindowSettings.h"

class EngineManager final : public Singleton<EngineManager>
{
public:
	EngineManager();
	virtual ~EngineManager();

	EngineManager(EngineManager& other) = delete;
	EngineManager(EngineManager&& other) = delete;
	EngineManager operator=(EngineManager& other) = delete;
	EngineManager& operator=(EngineManager&& other) = delete;

	inline EngineDevice& GetDevice(HWND windowHandle = nullptr, const WindowSettings* settings = nullptr)
	{
		if (!m_pDevice)
		{
			m_pDevice = new EngineDevice();
			m_pDevice->InitD3D(windowHandle, *settings);
		}
		return *m_pDevice;
	}
	inline Renderer& GetRenderer(EngineDevice* device = nullptr)
	{
		if (!m_pRenderer)
		{
			m_pRenderer = new Renderer(device);
		}
		return *m_pRenderer;
	}

private:
	static EngineDevice* m_pDevice;
	static Renderer* m_pRenderer;
};
