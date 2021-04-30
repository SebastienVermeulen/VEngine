#pragma once
#include "BaseUpdateStructure.h"

class Object;

class Scene final : public BaseUpdateStructure
{
public:
	Scene();
	~Scene();

	Scene(Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene operator=(Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;

	virtual void Init() override;
	virtual void Cleanup() override;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;
	virtual void Render() const override;

	Object* CreateObject();

private:
	std::vector<Object*> m_Objects;
};