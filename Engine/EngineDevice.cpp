#include "pch.h"
#include "EngineDevice.h"
#include "Window.h"
#include "ShaderStructures.h"

EngineDevice::EngineDevice()
    :m_pSwapChain{ nullptr }
    , m_pDevice{ nullptr }
    , m_pDeviceContext{ nullptr }
    , m_pRenderTargets{ }
    , m_pDepthStencilView{ nullptr }
    , m_pDepthStencilBuffer{ nullptr }
    , m_DefaultWidth{}
    , m_DefaultHeight{}
{
}
EngineDevice::~EngineDevice()
{
    CleanD3D();
}

void EngineDevice::InitD3D(HWND hWnd, const WindowSettings settings)
{
    HRESULT hr;
    m_DefaultWidth = settings.windowWidth;
    m_DefaultHeight = settings.windowHeight;

#pragma region Device
    //Create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    //Clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    //Fill the swap chain description struct
    scd.BufferCount = 1;                                                            //One back buffer
    scd.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;                //Use 32-bit color
    scd.BufferDesc.Width = m_DefaultWidth;                                          //Set the back buffer width
    scd.BufferDesc.Height = m_DefaultHeight;                                        //Set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                              //How swap chain is to be used
    scd.OutputWindow = hWnd;                                                        //The window to be used
    scd.SampleDesc.Count = 1;                                                       //How many multisamples
    scd.Windowed = TRUE;                                                            //Windowed/full-screen mode
    scd.Flags = settings.flags;                                                     //Windowed/full-screen mode

    //Create a device, device context and swap chain using the information in the scd struct
    hr = D3D11CreateDeviceAndSwapChain(NULL,
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

    if (hr != S_OK) 
    {
        //TO-DO: make logger
    }
#pragma endregion

#pragma region Rendertarget
    m_pRenderTargets.push_back(new RenderTarget());
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pRenderTargets[0]->pTexture);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }
    
    //Use the back buffer address to create the render target
    hr = m_pDevice->CreateRenderTargetView(m_pRenderTargets[0]->pTexture, nullptr, &m_pRenderTargets[0]->pRenderTargetView);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }
    
    hr = m_pDevice->CreateShaderResourceView(m_pRenderTargets[0]->pTexture, nullptr, &m_pRenderTargets[0]->pShaderResourceView);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }
#pragma endregion

#pragma region Viewport
    //Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)m_DefaultWidth;
    viewport.Height = (FLOAT)m_DefaultHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_pDeviceContext->RSSetViewports(1, &viewport);
#pragma endregion

#pragma region DepthBuffer
    //Describe our Depth/Stencil Buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.Width = m_DefaultWidth;
    depthStencilDesc.Height = m_DefaultHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);

    if (hr != S_OK)
    {
        //TO-DO: make logger
    }

    if (m_pDepthStencilBuffer != nullptr)
    {
        hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, &m_pDepthStencilView);
    }

    if (hr != S_OK)
    {
        //TO-DO: make logger
    }
#pragma endregion 

#pragma region ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
    ImGui::StyleColorsDark();
#pragma endregion
}
void EngineDevice::CleanD3D()
{
    //Switch to windowed mode
    m_pSwapChain->SetFullscreenState(FALSE, NULL); 

    //Close and release all existing COM objects
    SafeRelease(m_pDepthStencilBuffer);
    SafeRelease(m_pDepthStencilView);
    SafeDelete(m_pRenderTargets);
    SafeRelease(m_pSwapChain);
    SafeRelease(m_pDevice);
    SafeRelease(m_pDeviceContext);

    //Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

RenderTarget* EngineDevice::GetRenderTarget(int index) const
{
    if (m_pRenderTargets.size() > index)
    {
        return m_pRenderTargets[index];
    }
    return nullptr;
}
RenderTarget* EngineDevice::TryGetRenderTarget(int index, bool customDesc, D3D11_TEXTURE2D_DESC desc)
{
    if (m_pRenderTargets.size() > index)
    {
        return m_pRenderTargets[index];
    }
    int temp{};
    return GetNewRenderTarget(temp, customDesc, desc);
}
RenderTarget* EngineDevice::GetNewRenderTarget(int& index, bool customDesc, D3D11_TEXTURE2D_DESC desc)
{
    //Use the back buffer address to create the render target
    index = (int)m_pRenderTargets.size();
    m_pRenderTargets.push_back(new RenderTarget());

    if (!customDesc) 
    {
        //Describe our Depth/Stencil Buffer
        ZeroMemory(&desc, sizeof(desc));

        desc.Width = m_DefaultWidth;
        desc.Height = m_DefaultHeight;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
    }

    HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, &m_pRenderTargets[index]->pTexture);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = desc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hr = m_pDevice->CreateRenderTargetView(m_pRenderTargets[index]->pTexture, &renderTargetViewDesc, &m_pRenderTargets[index]->pRenderTargetView);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = desc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = m_pDevice->CreateShaderResourceView(m_pRenderTargets[index]->pTexture, &shaderResourceViewDesc, &m_pRenderTargets[index]->pShaderResourceView);
    if (hr != S_OK)
    {
        //TO-DO: make logger
    }

    return m_pRenderTargets[index];
}
