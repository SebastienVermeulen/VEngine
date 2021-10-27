#include "pch.h"
#include "App.h"
#include "Window.h"
#include "WindowSettings.h"
#include "Renderer.h"
#include "EngineManager.h"
#include "Project.h"
#include "AppTime.h"
#include "UIRenderer.h"

App::App(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings)
	:m_pWindow{}
    , m_pProject{}
    , m_pRenderer{}
{
	Init(hInstance, nCmdShow, settings);
}
App::~App() 
{
    EngineManager::ReleaseInstance();
    UIRenderer::ReleaseInstance();
    AppTime::ReleaseInstance();

    SafeDelete(m_pProject);
}

#pragma region InitAndClean
HRESULT App::Init(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings)
{
    EngineManager* pEngineManager = EngineManager::Instance();

    m_pWindow = pEngineManager->GetWindow(hInstance, settings, nCmdShow);
	if (m_pWindow == nullptr) 
	{
        //TO-DO: make logger
		return E_FAIL;
	}
    
	//Init the device through the locator
	EngineDevice* pDevice = pEngineManager->GetDevice(m_pWindow->GetHWND(), &settings);
	m_pRenderer = pEngineManager->GetRenderer(pDevice);
    if (m_pProject)
    {
        m_pProject->Init();
    }

	return S_OK;
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
            float deltaTime = AppTime::Instance()->Update();

            m_pProject->Update(deltaTime);

            if (AppTime::Instance()->FixedUpdate())
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
    if (m_pProject->Init()) 
    {
        m_pProject->MarkAsInit();
    }
    if (m_pProject->HasInit()) 
    {
        EngineManager::Instance()->SetProject(m_pProject);
    }
}
