#pragma once
#include "Material.h"
#include "EngineDevice.h"
#include "EngineManager.h"

class Renderable abstract
{
public:
	inline virtual void Render(ID3D11Device* device, ID3D11DeviceContext* context, int passNr = 0) const {}

	inline void SetMaterial(Material* pMat) 
	{
		m_pMaterial = pMat;

		if (m_pMaterial)
		{
			EngineDevice* pEngineDevice = EngineManager::Instance()->GetDevice();
			m_pMaterial->InitShader(pEngineDevice->GetDevice(), pEngineDevice->GetDeviceContext());
		}
	}
	inline Material* GetMaterial() const { return m_pMaterial; }

	inline bool ShouldRender() { return m_ShouldRender; }
	inline void ShouldRender(bool shouldRender) { m_ShouldRender = shouldRender; }

protected:
	inline Renderable() 
		:m_pMaterial{}
		, m_ShouldRender{false}
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
	bool m_ShouldRender;
};