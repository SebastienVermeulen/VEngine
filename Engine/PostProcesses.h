#pragma once
#include "Singleton.h"
#include "ScreenSpaceCopy.h"
#include "GreyScale.h"
#include "ShaderStructures.h"

class EngineDevice;
class Renderer;

class PostProcessSwapChain final : public Singleton<PostProcessSwapChain>
{
public:
	PostProcessSwapChain() 
		:m_TargetOne{ nullptr }
		, m_TargetTwo{ nullptr }
	{}
	~PostProcessSwapChain() {}

	PostProcessSwapChain(PostProcessSwapChain& other) = delete;
	PostProcessSwapChain(PostProcessSwapChain&& other) = delete;
	PostProcessSwapChain operator=(PostProcessSwapChain& other) = delete;
	PostProcessSwapChain& operator=(PostProcessSwapChain&& other) = delete;

	void SetTargets(RenderTarget* targetOne, RenderTarget* targetTwo)
	{
		m_TargetOne = targetOne;
		m_TargetTwo = targetTwo;
	}
	RenderTarget* GetTarget() 
	{
		return m_bSwap ? m_TargetTwo : m_TargetOne;
	}
	RenderTarget* GetSource()
	{
		return m_bSwap ? m_TargetOne : m_TargetTwo;
	}
	void Swap()
	{
		m_bSwap = !m_bSwap;
	}

private:
	RenderTarget* m_TargetOne;
	RenderTarget* m_TargetTwo;
	bool m_bSwap = true;
};

// This will become the structure that executes whatever is needed for the pipeline
// While PostProcessPipeline is simply our interface
// Potentially in the future this can make it really easy to swap out PostProcessPipelines
struct PostProcesses
{
	PostProcesses();
	~PostProcesses();

	void Init(EngineDevice* pDevice);
	void AllocatePromisedTargets(EngineDevice* pDevice);
	void DeallocatePromisedTargets();

	void RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer);

	ScreenSpaceCopy screenSpaceCopy;
	GreyScalePostProcess greyScalePostProcess;
};

// TO-DO: Specialize per renderer, will hurt maintainability, however not all techniques need to be supported on each renderer
class PostProcessPipeline 
{
public:
	PostProcessPipeline();
	virtual ~PostProcessPipeline();

	PostProcessPipeline(PostProcessPipeline& other) = delete;
	PostProcessPipeline(PostProcessPipeline&& other) = delete;
	PostProcessPipeline operator=(PostProcessPipeline& other) = delete;
	PostProcessPipeline& operator=(PostProcessPipeline&& other) = delete;

	virtual void Init(EngineDevice* pDevice);
	virtual void RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer);

	inline PostProcesses* GetPostProcesses()
	{
		return &m_PostProcesses;
	}

private:
	PostProcesses m_PostProcesses;
};