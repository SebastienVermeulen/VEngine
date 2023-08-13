#include "pch.h"
#include "ForwardsDX11.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "Component.h"
#include "UIRenderer.h"
#include "EngineSettings.h"

ForwardsDX11::ForwardsDX11(EngineDevice* device)
	:Renderer(device)
{
	m_RenderType = RenderType::forwards;
}
ForwardsDX11::~ForwardsDX11()
{
}

void ForwardsDX11::Render()
{
	//No camera == no rendering
	if (!m_pRenderingCamera)
	{
		return;
	}

	ClearBuffers();

	//**********************************************************************
	// 	   Main render loop
	//**********************************************************************
	for (int i = 0; i < m_Renderables.size(); ++i)
	{
		if (m_Renderables[i]->IsVisible())
		{
			//Set the parameters
			Material* pMaterial = m_Renderables[i]->GetMaterial();
			pMaterial->UpdateParameterValues(m_pDevice);
			if (m_UpdateLighting)
			{
				//Update the lights
				UpdateLights(m_Renderables[i]->GetMaterial());
			}
			//Update matrices
			UpdateMatrices(m_Renderables[i]);
			//Set the render target
			SetupTargetsForwardsPass();
			//Forwards pass
			m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
		}
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	//Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	UIRenderer::Instance()->RenderUI();

	//Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(m_pEngineSettings->GetIfVSync(), 0);
}
void ForwardsDX11::ClearBuffers()
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Clear the main target buffer to a deep blue
	pContext->ClearRenderTargetView(m_pDevice->GetRenderTarget(0)->pRenderTargetView, &m_DefaultClearColor.x);
	//Clear the other buffers as well with black
	RenderTarget* pRenderTarget;
	ID3D11RenderTargetView* pViewToClear;
	int index = 1;
	do
	{
		pRenderTarget = m_pDevice->GetRenderTarget(index);
		if (pRenderTarget)
		{
			pViewToClear = pRenderTarget->pRenderTargetView;
			const DirectX::XMFLOAT4 black = { 0,0,0,0 };
			pContext->ClearRenderTargetView(pViewToClear, &black.x);
			//Get the next render target
			++index;
		}
	} while (pRenderTarget && pViewToClear);
	//Clear the depth buffer to the max value
	pContext->ClearDepthStencilView(m_pDevice->GetDepthBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma region RenderHelpers
void ForwardsDX11::SetupTargetsForwardsPass() const
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Index 0 is guaranted to exists by design, but is the final render target (swapchain buffer)
	pContext->OMSetRenderTargets(1, &m_pDevice->GetRenderTarget(0)->pRenderTargetView, m_pDevice->GetDepthBuffer());
}
#pragma region
