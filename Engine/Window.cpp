#include "pch.h"
#include "Window.h"
#include "WindowSettings.h"

Window::Window(HINSTANCE hInstance, const WindowSettings& settings, const int nCmdShow)
    :m_HandleWindow{}
    , m_WindowSettings{ settings }
{
    Init(hInstance, nCmdShow);
}
Window::~Window()
{

}

LRESULT Window::Init(HINSTANCE hInstance, const int nCmdShow)
{
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window::WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = m_WindowSettings.backgroundBrush;
    wc.lpszClassName = L"WindowClass1";

    // register the window class
    RegisterClassEx(&wc);

    RECT rect = { 0, 0, (LONG)m_WindowSettings.windowWidth, (LONG)m_WindowSettings.windowHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window and use the result as the handle
    m_HandleWindow = CreateWindowEx(NULL,
        L"WindowClass1",    // name of the window class
        L"Our First Windowed Program",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style
        300,    // x-position of the window
        300,    // y-position of the window
        rect.right - rect.left,    // width of the window
        rect.bottom - rect.top,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        hInstance,    // application handle
        NULL);    // used with multiple windows, NULL

    // display the window on the screen
    return ShowWindow(m_HandleWindow, nCmdShow);
}

// this is the main message handler for the program
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch (message)
    {
    // this message is read when the window is closed
    case WM_DESTROY:
    {
        // close the application entirely
        PostQuitMessage(0);
        return 0;
    }
    break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL Window::AdjustWindowsRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
    UNREFERENCED_PARAMETER(lpRect);
    UNREFERENCED_PARAMETER(dwStyle);
    UNREFERENCED_PARAMETER(bMenu);
    return TRUE;
}
