#include "pch.h"
#include "Renderer.h"
#include "EngineManager.h"
#include "EngineDevice.h"

EngineDevice* Renderer::m_pDevice = nullptr;

Renderer::Renderer(EngineDevice* pDevice)
{
    m_pDevice = pDevice;
}
Renderer::~Renderer()
{
}

void Renderer::Render()
{
    //Clear the back buffer to a deep blue
    m_pDevice->GetDeviceContext().ClearRenderTargetView(&m_pDevice->GetBackBuffer(), D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

    //Do 3D rendering on the back buffer here

    //Switch the back buffer and the front buffer
    m_pDevice->GetSwapChain().Present(0, 0);
}
