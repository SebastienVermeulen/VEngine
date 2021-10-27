//Written by Sebastien Vermeulen | 14/04/2021
//VEngine - DirectX rasterisation framework

#include "pch.h"
#include "App.h"
#include "Project.h"
#include "MainScene.h"
#include "EngineManager.h"

//The entry point for any Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);

    //Create app
    WindowSettings windowSettings = WindowSettings::Windowed();
    windowSettings.windowWidth += 200; //Add 200 for details panel
    App pApp = App(hInstance, nCmdShow, windowSettings);

    //Create a project and scene
    Project* pProject = new Project();
    MainScene* pScene = pProject->CreateScene<MainScene>(true);

    //Open the project in the app and run the app
    pApp.OpenProject(pProject);
    int message = pApp.Run();

    return message;
}