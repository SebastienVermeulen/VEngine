#include "pch.h"
#include "App.h"
#include "Window.h"
#include "WindowSettings.h"
#include "Renderer.h"
#include "EngineManager.h"
#include "Project.h"
#include "AppTime.h"

App::App(HINSTANCE hInstance, const int nCmdShow) 
	:m_pWindow{}
    , m_pProject{}
    , m_pRenderer{}
{
	Init(hInstance, nCmdShow);
}
App::~App() 
{
    SafeDelete(m_pWindow);

    EngineManager::ReleaseInstance();

    SafeDelete(m_pProject);
}

#pragma region InitAndClean
LRESULT App::Init(HINSTANCE hInstance, const int nCmdShow)
{
    WindowSettings windowSettings = WindowSettings::Windowed();
	m_pWindow = new Window(hInstance, windowSettings, nCmdShow);
	if (m_pWindow == nullptr) 
	{
		return false;
	}

	//Init the device through the locator
	EngineDevice* pDevice = &EngineManager::Instance().GetDevice(m_pWindow->GetHWND(), &windowSettings);
	m_pRenderer = &EngineManager::Instance().GetRenderer(pDevice);

    if (m_pProject)
    {
        m_pProject->Init();
    }

	return true;
}
#pragma endregion

int App::Run()
{
    //This struct holds Windows event messages
    MSG msg;

    //Wait for the next message in the queue, store the result in 'msg'
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            //Translate keystroke messages into the right format
            TranslateMessage(&msg);

            //Send the message to the WindowProc function
            DispatchMessage(&msg);

            //Check to see if it's time to quit
            if (msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            float deltaTime = AppTime::Update();

            m_pProject->Update(deltaTime);

            if (AppTime::FixedUpdate())
            {
                m_pProject->FixedUpdate();
            }

            m_pProject->LateUpdate(deltaTime);

            //Run game code
            m_pRenderer->Render();
        }
    }

    //Return this part of the WM_QUIT message to Windows
    return int(msg.wParam);
}

void App::OpenProject(Project* project)
{
    m_pProject = project;
    m_pProject->Init();
}
