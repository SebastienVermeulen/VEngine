#pragma once
#include "BaseUpdateStructure.h"
#include "ComponentStructure.h"
#include "Component.h"

class Transform;
class ObjectWidget;

class Object : public BaseUpdateStructure, public ComponentStructure
{
public:
	Object(std::string& name);
	~Object();

	Object(Object& other) = delete;
	Object(Object&& other) = delete;
	Object operator=(Object& other) = delete;
	Object& operator=(Object&& other) = delete;

	virtual bool Init() override;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;

	inline Transform* GetTransform() const 
	{ 
		return m_pTransform; 
	}
	inline ObjectWidget* GetObjectWidget() const 
	{
		return m_pObjectWidget; 
	}

private:
	ObjectWidget* m_pObjectWidget;
	Transform* m_pTransform;
};
