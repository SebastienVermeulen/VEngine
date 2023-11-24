#include "pch.h"
#include "ForwardsDX11.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "Component.h"
#include "UIRenderer.h"
#include "EngineSettings.h"
#include "PostProcesses.h"
#include "RenderAnnotation.h"

ForwardsDX11::ForwardsDX11(EngineDevice* device)
	:Renderer(device)
{
	m_RenderType = RenderType::forwards;
}
ForwardsDX11::~ForwardsDX11()
{
}

void ForwardsDX11::Init() 
{
	Renderer::Init();
}

void ForwardsDX11::AllocatePromisedTargets()
{
	Renderer::AllocatePromisedTargets();
}

void ForwardsDX11::Render()
{
	//No camera == no rendering
	if (!m_pRenderingCamera)
	{
		return;
	}

	ClearBuffers();

	{
		V_DX11_ANNOTATE(V_WTEXT("Scene"));

		//**********************************************************************
		// 	   Main render loop
		//**********************************************************************
		//Set the render target
		SetupTargetsForwardsPass();

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
				//Forwards pass
				m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
			}
		}

		//Unhook render targets
		ExplicitlyUnbindingRenderTargets();
	}

	//**********************************************************************
	// 	   Postprocess
	//**********************************************************************
	{
		V_DX11_ANNOTATE(V_WTEXT("Postprocessing"));

		m_pPostProcessingPipeline->RunPostProcesses(m_pDevice, this);
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	//Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	{
		V_DX11_ANNOTATE(V_WTEXT("UI"));

		ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
		ID3D11RenderTargetView* renderTargs[] =
		{
			GetFinalTarget()->GetRenderTargetView()
		};
		pContext->OMSetRenderTargets(1, renderTargs, nullptr);
		UIRenderer::Instance()->RenderUI();
	}

	//Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(m_pEngineSettings->GetIfVSync(), 0);
}
void ForwardsDX11::ClearBuffers()
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	//Clear the main target buffer to a deep blue
	pContext->ClearRenderTargetView(GetFinalSceneTarget()->GetRenderTargetView(), &m_DefaultClearColor.x);
	//Clear the other buffers as well with black
	const DirectX::XMFLOAT4 black = { 0,0,0,0 };
	pContext->ClearRenderTargetView(GetFinalTarget()->GetRenderTargetView(), &black.x);
	//Clear the depth buffer to the max value
	pContext->ClearDepthStencilView(m_pDevice->GetDepthBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma region RenderHelpers
void ForwardsDX11::SetupTargetsForwardsPass()
{
	// Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTargs[] =
	{
		GetFinalSceneTarget()->GetRenderTargetView()
	};
	pContext->OMSetRenderTargets(1, renderTargs, m_pDevice->GetDepthBuffer());
}
#pragma region
