#include "pch.h"
#include "EngineDevice.h"
#include "Window.h"
#include "ShaderStructures.h"

EngineDevice::EngineDevice()
    :m_pSwapChain{ nullptr }
    , m_pDevice{ nullptr }
    , m_pDeviceContext{ nullptr }
    , m_RenderTargets{ }
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
    // Create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // Clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // Fill the swap chain description struct
    scd.BufferCount = 2;                                                            //One back buffer
    scd.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;                //Use 32-bit color
    scd.BufferDesc.Width = m_DefaultWidth;                                          //Set the back buffer width
    scd.BufferDesc.Height = m_DefaultHeight;                                        //Set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                              //How swap chain is to be used
    scd.OutputWindow = hWnd;                                                        //The window to be used
    scd.SampleDesc.Count = 1;                                                       //How many multisamples
    scd.Windowed = TRUE;                                                            //Windowed/full-screen mode
    scd.Flags = settings.flags;                                                     //Windowed/full-screen mode
    scd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;               //Type of swapchain method in use

    D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
    UINT numberOfLevels = 1;
    D3D_FEATURE_LEVEL featureLevelSupported;

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    // If the project is in a debug build, enable the debug layer.
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create a device, device context and swap chain using the information in the scd struct
    hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        creationFlags,
        &featureLevelRequested,
        numberOfLevels,
        D3D11_SDK_VERSION,
        &scd,
        &m_pSwapChain,
        &m_pDevice,
        &featureLevelSupported,
        &m_pDeviceContext);

    if (hr != S_OK) 
    {
        V_LOG(LogVerbosity::Fatal, V_WTEXT("EngineDevice: Failed making the Device and swapchain."));
    }
#pragma endregion

#pragma region Rendertarget
    m_RenderTargets.push_back(new RenderTarget());

    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_RenderTargets[0]->pTexture);
    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making the texture for the main rendertarget."));
    }
    
    // Use the back buffer address to create the render target
    hr = m_pDevice->CreateRenderTargetView(m_RenderTargets[0]->pTexture, nullptr, &m_RenderTargets[0]->pRenderTargetView);
    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making the main rendertarget."));
    }

    // We ignore the shader resource view as this is the backbuffer and thus should not be used as a shader resource.
    // Leave code here for future reference.
    // hr = m_pDevice->CreateShaderResourceView(m_pRenderTargets[0]->pTexture, nullptr, &m_pRenderTargets[0]->pShaderResourceView);
    // if (hr != S_OK)
    // {
    //     V_LOG(V_Fatal, V_WTEXT("EngineDevice: Failed making the main rendertarget ShaderResourceView."));
    // }
#pragma endregion

#pragma region Viewport
    // Set the viewport
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
    // Describe our Depth/Stencil Buffer
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
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making the texture for the DepthStencilView."));
    }

    if (m_pDepthStencilBuffer != nullptr)
    {
        hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, &m_pDepthStencilView);
    }

    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making the DepthStencilView."));
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
    // Switch to windowed mode
    m_pSwapChain->SetFullscreenState(FALSE, NULL); 

    // Close and release all existing COM objects
    SafeRelease(m_pDepthStencilBuffer);
    SafeRelease(m_pDepthStencilView);
    SafeDelete(m_RenderTargets);
    SafeRelease(m_pSwapChain);
    SafeRelease(m_pDevice);
    SafeRelease(m_pDeviceContext);

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

RenderTarget* EngineDevice::GetRenderTarget(int index) const
{
    if (m_RenderTargets.size() > index)
    {
        return m_RenderTargets[index];
    }
    return nullptr;
}
RenderTarget* EngineDevice::TryGetRenderTarget(int index, bool customDesc, D3D11_TEXTURE2D_DESC desc)
{
    if (m_RenderTargets.size() > index)
    {
        return m_RenderTargets[index];
    }
    int temp{};
    return GetNewRenderTarget(temp, customDesc, desc);
}
RenderTarget* EngineDevice::GetNewRenderTarget(int& index, bool customDesc, D3D11_TEXTURE2D_DESC desc)
{
    //Use the back buffer address to create the render target
    index = (int)m_RenderTargets.size();
    m_RenderTargets.push_back(new RenderTarget());

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

    HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, &m_RenderTargets[index]->pTexture);
    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making a texture for a RenderTarget."));
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = desc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hr = m_pDevice->CreateRenderTargetView(m_RenderTargets[index]->pTexture, &renderTargetViewDesc, &m_RenderTargets[index]->pRenderTargetView);
    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making a RenderTarget."));
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = desc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    hr = m_pDevice->CreateShaderResourceView(m_RenderTargets[index]->pTexture, &shaderResourceViewDesc, &m_RenderTargets[index]->pShaderResourceView);
    if (hr != S_OK)
    {
        V_LOG(LogVerbosity::Error, V_WTEXT("EngineDevice: Failed making a resourceview for a RenderTarget."));
    }

    return m_RenderTargets[index];
}

HRESULT EngineDevice::ReleaseTarget(int index) 
{
    if (m_RenderTargets.empty() == 0 || m_RenderTargets.size() >= index)
    {
        return E_FAIL;
    }
    
    SafeDelete(m_RenderTargets[index]);

    return S_OK;
}

HRESULT EngineDevice::CreateVertexBuffer(std::vector<Vertex>& vertices, ID3D11Buffer** ppVertexBuffer) const
{
    Vertex* verticesArray = &vertices[0];
    int verticesNr = (UINT)vertices.size();

    D3D11_SUBRESOURCE_DATA vertexMS;
    D3D11_BUFFER_DESC vertexBD;
    HRESULT result;

    //Vertex
    vertexBD.Usage = D3D11_USAGE_DEFAULT;
    vertexBD.ByteWidth = (UINT)sizeof(vertices[0]) * verticesNr;
    vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBD.CPUAccessFlags = 0;
    vertexBD.MiscFlags = 0;
    vertexBD.StructureByteStride = 0;

    //Give the subresource structure a pointer to the vertex data.
    vertexMS.pSysMem = verticesArray;
    vertexMS.SysMemPitch = 0;
    vertexMS.SysMemSlicePitch = 0;

    //Now create the vertex buffer.
    result = m_pDevice->CreateBuffer(&vertexBD, &vertexMS, ppVertexBuffer);
    if (FAILED(result))
    {
        V_LOG(LogVerbosity::Warning, V_WTEXT("EngineDevice: Failed making a vertexbuffer."));
    }
    return result;
}
HRESULT EngineDevice::CreateIndexBuffer(std::vector<unsigned int>& indices, ID3D11Buffer** ppIndexBuffer) const
{
    unsigned int* indicesArray = &indices[0];
    int indicesNr = (UINT)indices.size();

    D3D11_SUBRESOURCE_DATA indexMS;
    D3D11_BUFFER_DESC indexBD;
    HRESULT result;

    //Index
    indexBD.Usage = D3D11_USAGE_DEFAULT;
    indexBD.ByteWidth = sizeof(unsigned int) * indicesNr;
    indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBD.CPUAccessFlags = 0;
    indexBD.MiscFlags = 0;
    indexBD.StructureByteStride = 0;

    //Give the subresource structure a pointer to the index data.
    indexMS.pSysMem = indicesArray;
    indexMS.SysMemPitch = 0;
    indexMS.SysMemSlicePitch = 0;

    //Now create the index buffer.
    result = m_pDevice->CreateBuffer(&indexBD, &indexMS, ppIndexBuffer);
    if (FAILED(result))
    {
        V_LOG(LogVerbosity::Warning, V_WTEXT("EngineDevice: Failed making a indexbuffer."));
    }
    return result;
}
