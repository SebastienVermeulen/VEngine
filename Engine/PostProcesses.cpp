#include "pch.h"
#include "PostProcesses.h"
#include "RenderUtils.h"
#include "Renderer.h"
#include "ForwardsDX11.h"
#include "DeferredDX11.h"
#include "RenderAnnotation.h"

PostProcesses::PostProcesses()
{
}
PostProcesses::~PostProcesses()
{
	screenSpaceCopy.ReleaseShader();
	greyScalePostProcess.ReleaseShader();
}

void PostProcesses::Init(EngineDevice* pDevice)
{
	screenSpaceCopy.InitShader(pDevice);
	greyScalePostProcess.InitShader(pDevice);
}
void PostProcesses::AllocatePromisedTargets(EngineDevice* pDevice)
{
	D3D11_TEXTURE2D_DESC desc1;
	ZeroMemory(&desc1, sizeof(desc1));
	desc1.Width = pDevice->GetDefaultWidth();
	desc1.Height = pDevice->GetDefaultHeight();
	desc1.MipLevels = 1;
	desc1.ArraySize = 1;
	desc1.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc1.SampleDesc.Count = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.Usage = D3D11_USAGE_DEFAULT;
	desc1.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc1.CPUAccessFlags = 0;
	desc1.MiscFlags = 0;

	PostProcessSwapChain::Instance()->SetTargets(pDevice->TryGetRenderTarget(true, desc1), pDevice->TryGetRenderTarget(true, desc1));
}
void PostProcesses::DeallocatePromisedTargets()
{
	// TO-DO: Make this more easily replacable
	PostProcessSwapChain* pPostProcessSwapChain = PostProcessSwapChain::Instance();
	pPostProcessSwapChain->GetSource()->MarkAsUnused();
	pPostProcessSwapChain->GetTarget()->MarkAsUnused();

	PostProcessSwapChain::Instance()->SetTargets(nullptr, nullptr);
}

void PostProcesses::RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer)
{
	AllocatePromisedTargets(pDevice);

	// TO-DO: Make this more easily replacable
	PostProcessSwapChain* pPostProcessSwapChain = PostProcessSwapChain::Instance();

	{
		// Copy to the final buffer, so we also leave the base scene untouched for some techniques
		screenSpaceCopy.RunEffect(pDevice, EngineManager::Instance()->GetActiveRenderer()->GetFinalSceneTarget(), pPostProcessSwapChain->GetTarget());
	
		pPostProcessSwapChain->Swap();
	}
	
	if (greyScalePostProcess.IsActive())
	{
		greyScalePostProcess.RunEffect(pDevice);
	}
	
	// Copy to the final target
	screenSpaceCopy.RunEffect(pDevice, pPostProcessSwapChain->GetSource(), EngineManager::Instance()->GetActiveRenderer()->GetFinalTarget());

	DeallocatePromisedTargets();
}

PostProcessPipeline::PostProcessPipeline()
{
	PostProcessSwapChain::Instance();
}
PostProcessPipeline::~PostProcessPipeline()
{
	PostProcessSwapChain::ReleaseInstance();
}

void PostProcessPipeline::Init(EngineDevice* pDevice)
{
	m_PostProcesses.Init(pDevice);
}
void PostProcessPipeline::RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer)
{
	m_PostProcesses.RunPostProcesses(pDevice, pRenderer);
}