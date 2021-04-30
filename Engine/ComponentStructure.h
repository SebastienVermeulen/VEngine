#pragma once
class Component;

class ComponentStructure abstract 
{
public:
	virtual void AddComponent(Component* pObject);
	virtual void RemoveComponent(Component* pObject);

protected:
	ComponentStructure();
	virtual ~ComponentStructure();

	ComponentStructure(ComponentStructure& other) = delete;
	ComponentStructure(ComponentStructure&& other) = delete;
	ComponentStructure operator=(ComponentStructure& other) = delete;
	ComponentStructure& operator=(ComponentStructure&& other) = delete;

	std::vector<Component*> m_Components;
};