#pragma once
#include "Renderer.h"

class EngineDevice;
class Material;
class RendererWidget;

class DeferredDX11 final : public Renderer 
{
public:
	DeferredDX11(EngineDevice* device);
	~DeferredDX11();

	DeferredDX11(DeferredDX11& other) = delete;
	DeferredDX11(DeferredDX11&& other) = delete;
	DeferredDX11 operator=(DeferredDX11& other) = delete;
	DeferredDX11& operator=(DeferredDX11&& other) = delete;

	virtual void Render() override;
	virtual void ClearBuffers() override;

	Material* GetDeferredLightingPassMaterial() const;

protected:
	virtual void ExplicitlyUnbindingRenderTargets() const override;

private:
	void SetupTargetsDeferredFirstPass() const;
	void SetupTargetsDeferredSecondPass() const;

	void CreateNDCQuad();

private:
	Material* m_pDeferredLightingMaterial;

	ID3D11Buffer* m_pScreenQuadVertexBuffer, * m_pScreenQuadIndexBuffer;
	UINT m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies;
};