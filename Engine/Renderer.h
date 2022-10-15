#pragma once
#include "ShaderStructures.h"

class EngineDevice;
class Camera;
class Component;
class Material;
class Light;
class RendererWidget;

//TO-DO: Make multiple renderers for maintanability
//This includes splitting forwards and deferred
class Renderer final
{
public:
	Renderer(EngineDevice* device);
	~Renderer();

	Renderer(Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer operator=(Renderer& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	void Render();
	void ClearBuffers();

	void AddRenderable(Component* pRenderable);
	void RemoveRenderable(Component* pRenderable);

	inline void SetRendertype(RenderType renderType)
	{ 
		m_RenderType = renderType;
		//Make sure the other render type has up to date lighting info
		m_UpdateLighting = true;
	}
	inline RenderType GetRenderType() const
	{
		return m_RenderType;
	}	
	inline void SetFPSLimit(bool FPSLimit)
	{ 
		m_VSync = FPSLimit;
	}
	inline bool GetFPSLimit() const
	{
		return m_VSync;
	}
	inline RendererWidget* GetWidget() const
	{
		return m_pRendererWidget;
	}
	Material* GetDeferredLightingPassMaterial() const;
	Camera* GetRenderingCamera() const;
	inline bool GetShouldUpdateLighting() const
	{
		return	m_UpdateLighting;
	}
	inline void SetShouldUpdateLighting(bool updateLighting) 
	{
		m_UpdateLighting = updateLighting;
	}

protected:
#pragma region RenderablesStorage
	void NewCameraAddedToScene(Camera* pCamera);
	void CameraRemovedFromScene(Camera* pCamera);
	void NewLightAddedToScene(Light* pLight);
	void LightRemovedFromScene(Light* pLight);
#pragma endregion

#pragma region RenderHelpers
	void UpdateMatrices(Component* pRenderable) const;
	void UpdateMatrices(Material* pMaterial) const;
	void UpdateLights(Material* pMaterial);

	void SetupTargetsDeferredFirstPass() const;
	void SetupTargetsDeferredSecondPass() const;
	void SetupTargetsForwardsPass() const;

	void ExplicitlyUnbindingRenderTargets() const;
#pragma endregion

private:
	void RenderDeferred();
	void CreateNDCQuad();

	static EngineDevice* m_pDevice;

	RendererWidget* m_pRendererWidget;

	std::vector<Component*> m_Renderables;
	std::vector<Light*> m_Lights;
	std::vector<Camera*> m_pCameraList;
	Camera* m_pRenderingCamera;

	Material* m_pDeferredLightingMaterial;
	ID3D11Buffer* m_pScreenQuadVertexBuffer, * m_pScreenQuadIndexBuffer;
	UINT m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies;

	DirectX::XMFLOAT4 m_DefaultClearColor;

	RenderType m_RenderType;
	int m_NrBasisRenderTargets;
	//TO-DO: make Vsync not scummy
	bool m_VSync;
	bool m_UpdateLighting;
};
