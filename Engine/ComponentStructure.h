#pragma once
class Component;

class ComponentStructure abstract 
{
public:
	virtual Component* AddComponent(Component* pObject);
	virtual void RemoveComponent(Component* pObject);

	inline std::vector<Component*>& GetAllComponents() { return m_Components; }

protected:
	ComponentStructure();
	virtual ~ComponentStructure();

	ComponentStructure(ComponentStructure& other) = delete;
	ComponentStructure(ComponentStructure&& other) = delete;
	ComponentStructure operator=(ComponentStructure& other) = delete;
	ComponentStructure& operator=(ComponentStructure&& other) = delete;

	std::vector<Component*> m_Components;
};