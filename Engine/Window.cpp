#include "pch.h"
#include "Window.h"
#include "WindowSettings.h"
#include "App.h"

Window::Window(HINSTANCE hInstance, const WindowSettings& settings, const int nCmdShow)
    :m_HandleWindow {}
    , m_WindowSettings{ settings }
{
    Init(hInstance, nCmdShow);
}
Window::~Window()
{

}

LRESULT Window::Init(HINSTANCE hInstance, const int nCmdShow)
{
    //This struct holds information for the window class
    WNDCLASSEX wc;

    //Clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    //Fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window::WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = m_WindowSettings.backgroundBrush;
    wc.lpszClassName = L"WindowClass1";

    //Register the window class
    RegisterClassEx(&wc);

    RECT rect = { 0, 0, (LONG)m_WindowSettings.windowWidth, (LONG)m_WindowSettings.windowHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    //Create the window and use the result as the handle
    m_HandleWindow = CreateWindowEx(NULL,
        L"WindowClass1",                        //Name of the window class
        L"VEngine",                             //Title of the window
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,   //Window style
        300,                                    //X-position of the window
        100,                                    //Y-position of the window
        rect.right - rect.left,                 //Width of the window
        rect.bottom - rect.top,                 //Height of the window
        NULL,                                   //We have no parent window, NULL
        NULL,                                   //We aren't using menus, NULL
        hInstance,                              //Application handle
        NULL);                                  //Used with multiple windows, NULL

    //Display the window on the screen
    return ShowWindow(m_HandleWindow, nCmdShow);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//This is the main message handler for the program
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    //Sort through and find what code to run for the message given
    switch (message)
    {
    //This message is read when the window is closed
    case WM_DESTROY:
    {
        //Close the application entirely
        PostQuitMessage(0);
        return 0;
    }
    break;
    }

    //Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL Window::AdjustWindowsRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
    UNREFERENCED_PARAMETER(lpRect);
    UNREFERENCED_PARAMETER(dwStyle);
    UNREFERENCED_PARAMETER(bMenu);
    return TRUE;
}
