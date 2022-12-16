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

	void ExplicitlyUnbindingRenderTargets() const;
#pragma endregion

protected:
	static EngineDevice* m_pDevice;

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
