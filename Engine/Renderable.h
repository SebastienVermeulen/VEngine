#pragma once
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"

class Renderable abstract
{
public:
	inline virtual void Render(ID3D11Device* device, ID3D11DeviceContext* context, int passNr = 0) const {}

	virtual inline void SetMaterial(Material* pMat)
	{
		m_pMaterial = pMat;

		if (m_pMaterial)
		{
			EngineDevice* pEngineDevice = EngineManager::Instance()->GetDevice();
			m_pMaterial->InitShader(pEngineDevice->GetDevice(), pEngineDevice->GetDeviceContext());
		}
	}
	inline Material* GetMaterial() const { return m_pMaterial; }
	inline std::vector<RenderType>& GetRenderTypes() { return m_RenderTypes; }

	inline bool CanRender() const { return m_CanRender; }
	inline void SetCanRender(bool canRender) { m_CanRender = canRender; }	
	inline bool IsVisible() const { return m_Visible; }
	inline void SetVisible(bool visible) { m_Visible = visible; }

	// The following 4 functions should only update internal types
	inline void SetRenderType(RenderType type)
	{
		// A bit lazy
		ClearRenderTypes();
		AddRenderType(type);
	}
	inline void ClearRenderTypes()
	{
		for (RenderType type : m_RenderTypes)
		{
			RemoveRenderType(type);
		}
		m_RenderTypes.clear();
	}
	inline void AddRenderType(RenderType type)
	{
		if (std::find(m_RenderTypes.begin(), m_RenderTypes.end(), type) == m_RenderTypes.end())
		{
			m_RenderTypes.push_back(type);
		}
	}
	inline void RemoveRenderType(RenderType type)
	{
		m_RenderTypes.erase(std::find(m_RenderTypes.begin(), m_RenderTypes.end(), type));
	}

protected:
	inline Renderable() 
		:m_pMaterial{}
		, m_CanRender{false}
		, m_Visible{true}
	{
	}
	inline virtual ~Renderable()
	{
		SafeDelete(m_pMaterial);
	}

	Renderable(Renderable& other) = delete;
	Renderable(Renderable&& other) = delete;
	Renderable operator=(Renderable& other) = delete;
	Renderable& operator=(Renderable&& other) = delete;

	Material* m_pMaterial;
	std::vector<RenderType> m_RenderTypes;
	bool m_CanRender;
	bool m_Visible;
};