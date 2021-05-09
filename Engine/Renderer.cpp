#include "pch.h"
#include "Renderer.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "Renderable.h"

EngineDevice* Renderer::m_pDevice = nullptr;

Renderer::Renderer(EngineDevice* pDevice)
    :m_Renderables{}
{
    m_pDevice = pDevice;
}
Renderer::~Renderer()
{
	for (int i = 0; i < m_Renderables.size(); i++)
	{
		m_Renderables[i] = nullptr;
	}
	m_Renderables.clear();
}

void Renderer::Render()
{
    //Clear the back buffer to a deep blue
    m_pDevice->GetDeviceContext().ClearRenderTargetView(&m_pDevice->GetBackBuffer(), D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

    //Do 3D rendering on the back buffer here
	for (int i = 0; i < m_Renderables.size(); i++)
	{
		m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext());
	}

    //Switch the back buffer and the front buffer
    m_pDevice->GetSwapChain().Present(0, 0);
}

void Renderer::AddRenderable(Renderable* pRenderable)
{
	if (std::find(m_Renderables.begin(), m_Renderables.end(), pRenderable) == m_Renderables.end())
	{
		m_Renderables.push_back(pRenderable);
	}
}
void Renderer::RemoveRenderable(Renderable* pRenderable)
{
	if (m_Renderables.size())
	{
		std::vector<Renderable*>::iterator it = std::find(m_Renderables.begin(), m_Renderables.end(), pRenderable);
		if (it != m_Renderables.end())
		{
			m_Renderables.erase(it);
		}
	}
}
