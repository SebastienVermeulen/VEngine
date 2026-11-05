#pragma once
#include "Scene.h"

class Object;
class DirectionalLight;
class PointLight;
class Material;

class MainScene final : public Scene
{
public:
	MainScene();
	virtual ~MainScene();

	MainScene(MainScene& other) = delete;
	MainScene(MainScene&& other) = delete;
	MainScene operator=(MainScene& other) = delete;
	MainScene& operator=(MainScene&& other) = delete;

	virtual bool Init() override;
	virtual void Update(const float elapsedSec) override;

private:
	DirectX::XMFLOAT3 m_CurrentRotation;

	DirectionalLight* m_pDirectionalLight;
	PointLight* m_pPointLight;

	std::vector<Material*> m_Materials;
};