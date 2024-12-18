#pragma once
#include "Renderer.h"

class EngineDevice;
class Material;
class RendererWidget;

struct DeferredRenderTargets 
{
	RenderTargets* m_pRenderTargets;

	RenderTarget* m_PositionTarget;
	RenderTarget* m_NormalTarget;
	RenderTarget* m_TangentTarget;
	RenderTarget* m_AlbedoTarget;
	RenderTarget* m_MetalRoughnessTarget;

	bool m_IsAssigned = false;
};

class DeferredDX11 final : public Renderer 
{
public:
	DeferredDX11(EngineDevice* device);
	~DeferredDX11();

	DeferredDX11(DeferredDX11& other) = delete;
	DeferredDX11(DeferredDX11&& other) = delete;
	DeferredDX11 operator=(DeferredDX11& other) = delete;
	DeferredDX11& operator=(DeferredDX11&& other) = delete;

	/* Renderer Interface */ 
	virtual void Init() override;

	virtual void AllocatePromisedTargets() override;
	virtual void CleanPointersToTargets() override;
	virtual void MarkTargetsAsNotUsed();

	virtual void Render() override;
	virtual void ClearBuffers() override;
	/* Renderer Interface */

	void RenderDefferedPass();
	Material* GetDeferredLightingPassMaterial() const;
	inline DeferredRenderTargets GetDeferredRenderTargets() const
	{
		return m_DeferredRenderTargets;
	}

private:
	void SetupTargetsDeferredFirstPass();
	void SetupTargetsDeferredSecondPass();

private:
	DeferredRenderTargets m_DeferredRenderTargets;
	Material* m_pDeferredLightingMaterial;

	ID3D11Buffer* m_pScreenQuadVertexBuffer, * m_pScreenQuadIndexBuffer;
	UINT m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies;
};