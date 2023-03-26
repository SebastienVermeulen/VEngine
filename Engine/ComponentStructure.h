#pragma once
class Component;
class EngineManager;
enum class RenderType : unsigned int;

class ComponentStructure abstract 
{
public:
	virtual Component* AddComponent(Component* pObject);
	virtual void RemoveComponent(Component* pObject);

	inline std::vector<Component*>& GetAllComponents() { return m_Components; }

	void SetRenderType(Component* pObject, RenderType type);
	void ClearRenderTypes(Component* pObject);
	void AddRenderType(Component* pObject, RenderType type);
	void RemoveRenderType(Component* pObject, RenderType type);

protected:
	ComponentStructure();
	virtual ~ComponentStructure();

	ComponentStructure(ComponentStructure& other) = delete;
	ComponentStructure(ComponentStructure&& other) = delete;
	ComponentStructure operator=(ComponentStructure& other) = delete;
	ComponentStructure& operator=(ComponentStructure&& other) = delete;

	std::vector<Component*> m_Components;

private:
	//void SetRenderType(EngineManager* pEngineManager, Component* pObject, RenderType type);
	//void ClearRenderTypes(EngineManager* pEngineManager, Component* pObject);
	void AddRenderType(EngineManager* pEngineManager, Component* pObject, RenderType type);
	void RemoveRenderType(EngineManager* pEngineManager, Component* pObject, RenderType type);
};