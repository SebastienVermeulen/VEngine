#include "pch.h"
#include "ComponentStructure.h"
#include "Object.h"

ComponentStructure::ComponentStructure()
	:m_Components{}
{
}
ComponentStructure::~ComponentStructure()
{
	SafeDelete(m_Components);
}

void ComponentStructure::AddComponent(Component* pComponent)
{
	if (m_Components.size())
	{
		if (std::find(m_Components.begin(), m_Components.end(), pComponent) == m_Components.end())
		{
			m_Components.push_back(pComponent);
		}
	}
}
void ComponentStructure::RemoveComponent(Component* pComponent)
{
	if (m_Components.size())
	{
		std::vector<Component*>::iterator it = std::find(m_Components.begin(), m_Components.end(), pComponent);
		if (it != m_Components.end())
		{
			m_Components.erase(it);
		}
	}
}
