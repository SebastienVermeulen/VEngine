#pragma once
#include "Component.h"

class Material;
class MeshAsset;

class MeshComponent : public Component
{
public:
	MeshComponent(const std::wstring& fileName);
	~MeshComponent();

	MeshComponent(MeshComponent& other) = delete;
	MeshComponent(MeshComponent&& other) = delete;
	MeshComponent operator=(MeshComponent& other) = delete;
	MeshComponent& operator=(MeshComponent&& other) = delete;

	virtual void SetMaterial(Material* pMat) override;

	virtual bool Init() override;

	virtual void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int passNr = 0) const override;
	virtual void RenderShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ShadowCasting* pShadowCasting, int passNr = 0) const override;

private:
	MeshAsset* m_pAsset;
	std::wstring m_FileName;

	ID3D11Buffer* m_pVertexBuffer, * m_pIndexBuffer;
	UINT m_NrVerticies, m_NrIndicies;
};
