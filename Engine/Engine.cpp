//Written by Sebastien Vermeulen | 14/04/2021
//VEngine - DirectX rasterisation framework

#include "pch.h"
#include "App.h"
#include "Project.h"

// the entry point for any Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);

    Project project{};
    project.CreateScene(true);

    //Run app and cleanup afterwards
    App* pApp = new App(hInstance, nCmdShow);
    pApp->OpenProject(project);
    int message = pApp->Run();
    SafeDelete(pApp);

    return message;
}
