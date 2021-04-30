#pragma once
#include "pch.h"

struct WindowSettings final
{
public:
	static inline WindowSettings Windowed()
	{
		WindowSettings settings{};
		settings.windowWidth = 800;
		settings.windowHeight = 600;
		settings.backgroundBrush = (HBRUSH)COLOR_WINDOW;
		settings.flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		return settings;
	}
	static inline WindowSettings FullScreen()
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		WindowSettings settings{};
		settings.windowWidth = desktop.right;
		settings.windowHeight = desktop.bottom;
		settings.backgroundBrush = (HBRUSH)NULL;
		settings.flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		return settings;
	}

	UINT windowWidth;
	UINT windowHeight;
	HBRUSH backgroundBrush;
	DXGI_SWAP_CHAIN_FLAG flags;
};
