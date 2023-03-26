#pragma once
#include "ShaderStructures.h"

class EngineDevice;
class EngineSettings;
class Camera;
class Component;
class Material;
class Light;
class RendererWidget;

class Renderer abstract
{
public:
	Renderer(EngineDevice* device);
	~Renderer();

	Renderer(Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer operator=(Renderer& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	virtual void Render() = 0;
	virtual void ClearBuffers() = 0;

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

	// DX11 is lazy by design, once needed it will implicitly unbind
	// Good practice is to unbind yourself once you are done so the device doesn't have to
	// Data might change from input to output or opposite then you need to act
	// https://stackoverflow.com/questions/52966262/id3dx11effectshaderresourcevariablesetresourcenull-cant-unbind-resources
	// https://www.gamedev.net/forums/topic/601013-directx11-multiple-render-target/
	virtual void ExplicitlyUnbindingRenderTargets() const;
#pragma endregion

protected:
	static EngineDevice* m_pDevice;
	static EngineSettings* m_pEngineSettings;

	RendererWidget* m_pRendererWidget;

	std::vector<Component*> m_Renderables;
	std::vector<Light*> m_Lights;
	std::vector<Camera*> m_pCameraList;
	Camera* m_pRenderingCamera;

	DirectX::XMFLOAT4 m_DefaultClearColor;

	RenderType m_RenderType;
	int m_NrBasisRenderTargets;
	bool m_UpdateLighting;
};
