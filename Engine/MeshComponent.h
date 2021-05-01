#pragma once
#include "Component.h"

class Shader;

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
	virtual void Cleanup() override;

private:
	Shader* m_pShader;
};
