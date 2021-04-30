#include "pch.h"
#include "EngineDevice.h"
#include "Window.h"

EngineDevice::EngineDevice()
    :m_pSwapChain{ nullptr }
    , m_pDevice{ nullptr }
    , m_pDeviceContext{ nullptr }
    , m_pBackbuffer{ nullptr }
{
}
EngineDevice::~EngineDevice()
{
    CleanD3D();
}

void EngineDevice::InitD3D(HWND hWnd, const WindowSettings settings)
{
#pragma region Device
    //Create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    //Clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    //Fill the swap chain description struct
    scd.BufferCount = 1;                                    //One back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     //Use 32-bit color
    scd.BufferDesc.Width = settings.windowWidth;            //Set the back buffer width
    scd.BufferDesc.Height = settings.windowHeight;          //Set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      //How swap chain is to be used
    scd.OutputWindow = hWnd;                                //The window to be used
    scd.SampleDesc.Count = 4;                               //How many multisamples
    scd.Windowed = TRUE;                                    //Windowed/full-screen mode
    scd.Flags = settings.flags;     //Windowed/full-screen mode

    //Create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &m_pSwapChain,
        &m_pDevice,
        NULL,
        &m_pDeviceContext);
#pragma endregion

#pragma region Rendertarget
    //Get the address of the backbuffer
    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    //Use the back buffer address to create the render target
    m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackbuffer);
    pBackBuffer->Release();

    //Set the render target as the backbuffer
    m_pDeviceContext->OMSetRenderTargets(1, &m_pBackbuffer, NULL);
#pragma endregion

#pragma region Viewport
    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)settings.windowWidth;
    viewport.Height = (FLOAT)settings.windowHeight;

    m_pDeviceContext->RSSetViewports(1, &viewport);
#pragma endregion
}
void EngineDevice::CleanD3D()
{
    m_pSwapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    SafeRelease(m_pSwapChain);
    SafeRelease(m_pDevice);
    SafeRelease(m_pDeviceContext);
    SafeRelease(m_pBackbuffer);
}
