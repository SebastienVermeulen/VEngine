#pragma once
#include "Component.h"

class Material;
class SkinnedMeshAsset;
class SkeletonAsset;

class SkeletalMeshComponent : public Component
{
public:
	SkeletalMeshComponent(const std::wstring& fileName);
	~SkeletalMeshComponent();

	SkeletalMeshComponent(SkeletalMeshComponent& other) = delete;
	SkeletalMeshComponent(SkeletalMeshComponent&& other) = delete;
	SkeletalMeshComponent operator=(SkeletalMeshComponent& other) = delete;
	SkeletalMeshComponent& operator=(SkeletalMeshComponent&& other) = delete;

	virtual void SetMaterial(Material* pMat) override;

	virtual bool Init() override;

	virtual void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int passNr = 0) const override;

private:
	SkinnedMeshAsset* m_pMeshAsset;
	SkeletonAsset* m_pSkeletonAsset;
	std::wstring m_FileName;

	ID3D11Buffer* m_pVertexBuffer, * m_pIndexBuffer;
	UINT m_NrVerticies, m_NrIndicies;
};
