#pragma once
#include "Component.h"

class Material;

class MeshComponent : public Component
{
public:
	MeshComponent();
	~MeshComponent();

	MeshComponent(MeshComponent& other) = delete;
	MeshComponent(MeshComponent&& other) = delete;
	MeshComponent operator=(MeshComponent& other) = delete;
	MeshComponent& operator=(MeshComponent&& other) = delete;

	virtual void Init() override;

	virtual void Render(ID3D11Device& device, ID3D11DeviceContext& context) const override;

	void SetMaterial(Material* pMat);

private:
	Material* m_pMaterial;

	ID3D11Buffer* m_pVertexBuffer;
	int m_NrVerticies;
};
