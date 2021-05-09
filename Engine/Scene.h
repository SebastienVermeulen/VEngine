#pragma once
#include "BaseUpdateStructure.h"

class Object;

class Scene : public BaseUpdateStructure
{
public:
	Scene();
	~Scene();

	Scene(Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene operator=(Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;

	Object* CreateObject();

private:
	std::vector<Object*> m_Objects;
};