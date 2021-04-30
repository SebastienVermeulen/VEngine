#pragma once
#include "BaseUpdateStructure.h"
#include "ObjectStructure.h"
#include "ComponentStructure.h"
#include "Component.h"

class Object : public BaseUpdateStructure, public ObjectStructure, public ComponentStructure
{
public:
	Object();
	~Object();

	Object(Object& other) = delete;
	Object(Object&& other) = delete;
	Object operator=(Object& other) = delete;
	Object& operator=(Object&& other) = delete;

	virtual void Init() override;
	virtual void Cleanup() override;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;
	virtual void Render() const override;
};
