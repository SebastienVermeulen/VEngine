#include "pch.h"
#include "PostProcesses.h"
#include "RenderUtils.h"
#include "Renderer.h"
#include "ForwardsDX11.h"
#include "DeferredDX11.h"

PostProcesses::PostProcesses()
{
}
PostProcesses::~PostProcesses()
{
	greyScalePostProcess.ReleaseShader();
}

void PostProcesses::Init(EngineDevice* pDevice)
{
	greyScalePostProcess.InitShader(pDevice);
}
void PostProcesses::RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer)
{
	if (greyScalePostProcess.IsActive())
	{
		greyScalePostProcess.SetupEffectEnvironment(pDevice);
		greyScalePostProcess.RunEffect(pDevice);
		greyScalePostProcess.TeardownEffectEnvironment(pDevice);

		//Unhook render targets from material
		pRenderer->ExplicitlyUnbindingRenderTargets(greyScalePostProcess.GetMaterial());
	}
}

PostProcessPipeline::PostProcessPipeline()
{
}
PostProcessPipeline::~PostProcessPipeline()
{
}

void PostProcessPipeline::Init(EngineDevice* pDevice)
{
	m_PostProcesses.Init(pDevice);
}
void PostProcessPipeline::RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer)
{
	m_PostProcesses.RunPostProcesses(pDevice, pRenderer);
}