#pragma once
#include "Singleton.h"
#include "WindowSettings.h"

class Window final
{
public:
	Window(HINSTANCE hInstance, const WindowSettings& settings, const int nCmdShow);
	~Window();

	Window(Window& other) = delete;
	Window(Window&& other) = delete;
	Window operator=(Window& other) = delete;
	Window& operator=(Window&& other) = delete;

	inline HWND GetHWND() const { return m_HandleWindow; }

private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT Init(HINSTANCE hInstance, const int nCmdShow);

	BOOL AdjustWindowsRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);

	// the handle for the window, filled by a function
	HWND m_HandleWindow;
	WindowSettings m_WindowSettings;
};
