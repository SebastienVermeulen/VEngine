#include "pch.h"
#include "App.h"
#include "Window.h"
#include "Renderer.h"
#include "EngineManager.h"
#include "Project.h"
#include "AppTime.h"
#include "UIRenderer.h"
#include "EngineSettings.h"

App::App(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings)
	:m_pProject{}
    , m_MaxMessagesPerTick{ 10 }
    , m_Shutdown{ false }
{
    V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: Initializations. ---"));

    if (Init(hInstance, nCmdShow, settings) != S_OK) 
    {
        V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: Failed Initializations. ---\nForcing shutdown."));
        m_Shutdown = true;
        return;
    }

    V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: Completed Initializations. ---"));
}
App::~App() 
{
    if (Cleanup() != S_OK) 
    {
        V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: Cleanup remains unfinished. ---"));
    }

    V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: Finished cleanup. ---"));
}

#pragma region InitAndClean
HRESULT App::Init(HINSTANCE hInstance, const int nCmdShow, WindowSettings settings)
{
    EngineManager* pEngineManager = EngineManager::Instance();

    //TO-DO: Engine settigns should be adjustable from some sort of ini file
    //Apply basic settings
    EngineSettings* pEngineSettings = EngineSettings::Instance();
    pEngineSettings->SetVSync(true);
    pEngineSettings->SetRendertypeClean(RenderType::deferred);

    Window* pWindow = pEngineManager->GetWindow(hInstance, settings, nCmdShow);
	if (pWindow == nullptr)
	{
        V_LOG(LogVerbosity::Fatal, V_WTEXT("App: Failed to get window."));
		return E_FAIL;
	}
    
	//Init the device through the locator
	EngineDevice* pDevice = pEngineManager->GetDevice(pWindow->GetHWND(), &settings);
    if (m_pProject)
    {
        m_pProject->Init();
    }

    //TO-DO: Engine settings should not hold lighting states, only settings, this needs to be moved to a more temporary structure
    //Force lighting to be recalculated
    pEngineSettings->MarkAllLightingDirty(pEngineManager);

	return S_OK;
}
HRESULT App::Cleanup()
{
    EngineSettings::ReleaseInstance();
    EngineManager::ReleaseInstance();
    UIRenderer::ReleaseInstance();
    AppTime::ReleaseInstance();

    return S_OK;
}
#pragma endregion

int App::Run()
{
    EngineManager* pEngineManager = EngineManager::Instance();

    //This struct holds Windows event messages
    MSG msg{};

    //Wait for the next message in the queue, store the result in 'msg'
    while (!m_Shutdown)
    {
        float deltaTime = AppTime::Instance()->Update();

        m_pProject->Update(deltaTime);

        if (AppTime::Instance()->FixedUpdate())
        {
            m_pProject->FixedUpdate();
        }

        m_pProject->LateUpdate(deltaTime);

        //Run game code
        pEngineManager->Render();

        //Resolve after update, to make the quit event instant
        ResolveMessages(msg);
    }

    //Return this part of the WM_QUIT message to Windows
    return int(msg.wParam);
}
void App::ResolveMessages(MSG& msg)
{
    size_t nrMessagesThisTick{};

    //Might be overkill to have a while here, just wanted to be sure
    //There could be multiple but they aren't queued, also QUIT will always run first
    while ((nrMessagesThisTick < m_MaxMessagesPerTick) && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        //Translate keystroke messages into the right format
        TranslateMessage(&msg);

        //Send the message to the WindowProc function
        DispatchMessage(&msg);

        //Check to see if it's time to quit
        if (msg.message == WM_QUIT)
        {
            m_Shutdown = true;

            V_LOG(LogVerbosity::Core, V_WTEXT("--- VEngine: User requested closing. ---"));
            //Skip all other code as the main running loop should end, per request of the user
            //Additional code for cleanup should be in destructors or post run
            return;
        }

        ++nrMessagesThisTick;
    }
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
