#pragma once
#include "ShaderStructures.h"

class EngineDevice;
class EngineSettings;
class Camera;
class Component;
class Material;
class Light;
class RendererWidget;
class ShadowCasting;
class PostProcessPipeline;

struct RenderTargets
{
	RenderTarget* m_FinalTarget;
	RenderTarget* m_FinalSceneTarget;

	DepthStencil* m_DepthBuffer;
};

class Renderer abstract
{
public:
	Renderer(EngineDevice* device);
	~Renderer();

	Renderer(Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer operator=(Renderer& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	virtual void Init();

	virtual void Render() = 0;
	virtual void ClearBuffers() = 0;

	/*
		Principly we provide targets just in time.
		So, we don't have to worry about it's continuous memory occupation.
		
		This function should be called at the start of this renderers
		frame, it request the required targets from the API. 
	*/
	virtual void AllocatePromisedTargets();
	/*
		Since we are using targets that aren't managed by these classes
		we need to clear our pointers when we no longer use the class.

		Otherwise it might be dangerous.
	*/
	virtual void CleanPointersToTargets();

	virtual void OponActivation();
	virtual void OponDeactivation();

	void AddRenderable(Component* pRenderable);
	void RemoveRenderable(Component* pRenderable);
	inline std::vector<Component*>& GetRenderables() 
	{
		return m_Renderables;
	}

	inline RenderType GetRenderType() const
	{
		return m_RenderType;
	}
	inline RendererWidget* GetWidget() const
	{
		return m_pRendererWidget;
	}
	inline Camera* GetRenderingCamera() const
	{
		return m_pRenderingCamera;
	}
	inline PostProcessPipeline* GetPostProcessPipeline()
	{
		return m_pPostProcessingPipeline;
	}	
	inline bool GetShouldUpdateLighting() const
	{
		return	m_UpdateLighting;
	}
	inline void SetShouldUpdateLighting(bool updateLighting) 
	{
		m_UpdateLighting = updateLighting;
	}

	inline RenderTargets& GetRenderTargets()
	{
		return m_RenderTargets;
	}
	inline RenderTarget* GetFinalTarget()
	{
		return m_RenderTargets.m_FinalTarget;
	}
	inline RenderTarget* GetFinalSceneTarget()
	{
		return m_RenderTargets.m_FinalSceneTarget;
	}	
	inline DepthStencil* GetSceneDepthBuffer()
	{
		return m_RenderTargets.m_DepthBuffer;
	}

#pragma region RenderHelpers
	// TO-DO: Make a generic render-independant function
	// DX11 is lazy by design, once needed it will implicitly unbind
	// Good practice is to unbind yourself once you are done so the device doesn't have to
	// Data might change from input to output or opposite then you need to act
	// https://stackoverflow.com/questions/52966262/id3dx11effectshaderresourcevariablesetresourcenull-cant-unbind-resources
	// https://www.gamedev.net/forums/topic/601013-directx11-multiple-render-target/
	virtual void ExplicitlyUnbindingRenderTargets(Material* pMaterial = nullptr);
#pragma endregion

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
	void UpdateLightMatrices(Light* pLight) const;
	void UpdateWorldMatrix(Component* pRenderables) const;
	void UpdateLights(Material* pMaterial) const;
	// TO-DO: This render stuff should be moved to the ShadowCasting class
	void RenderShadows() const;
	void UpdateShadows(Material* pMaterial) const;
#pragma endregion

protected:
	static EngineDevice* m_pDevice;
	static EngineSettings* m_pEngineSettings;

	RendererWidget* m_pRendererWidget;

	RenderTargets m_RenderTargets;

	std::vector<Component*> m_Renderables;
	std::vector<Light*> m_Lights;
	std::vector<Camera*> m_pCameraList;
	Camera* m_pRenderingCamera;
	ShadowCasting* m_pShadowCasting;
	PostProcessPipeline* m_pPostProcessingPipeline;

	DirectX::XMFLOAT4 m_DefaultClearColor;

	RenderType m_RenderType;
	int m_NrBasisRenderTargets;
	bool m_UpdateLighting;
	bool m_RenderShadows;
};
