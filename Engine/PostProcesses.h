#pragma once
#include "GreyScale.h"

class EngineDevice;
class Renderer;

// This will become the structure that executes whatever is needed for the pipeline
// While PostProcessPipeline is simply our interface
// Potentially in the future this can make it really easy to swap out PostProcessPipelines
struct PostProcesses
{
	PostProcesses();
	~PostProcesses();

	void Init(EngineDevice* pDevice);
	void RunPostProcesses(EngineDevice* pDevice, Renderer* pRenderer);

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