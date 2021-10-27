#include "pch.h"
#include "EngineManager.h"
#include "Renderer.h"
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

Component* ComponentStructure::AddComponent(Component* pComponent)
{
	if (std::find(m_Components.begin(), m_Components.end(), pComponent) == m_Components.end())
	{
		m_Components.push_back(pComponent);

		if (!pComponent->HasInit())
		{
			pComponent->Init();
			pComponent->MarkAsInit();
		}
		if (pComponent->ShouldRender())
		{
			EngineManager::Instance()->GetRenderer()->AddRenderable(pComponent);
		}

		pComponent->SetObject(static_cast<Object*>(this));
		return pComponent;
	}
	return nullptr;
}
void ComponentStructure::RemoveComponent(Component* pComponent)
{
	if (m_Components.size())
	{
		std::vector<Component*>::iterator it = std::find(m_Components.begin(), m_Components.end(), pComponent);
		if (it != m_Components.end())
		{
			m_Components.erase(it);

			if (pComponent->ShouldRender())
			{
				EngineManager::Instance()->GetRenderer()->RemoveRenderable(pComponent);
			}

			pComponent->SetObject(nullptr);
		}
	}
}
