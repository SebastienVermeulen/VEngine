#include "pch.h"
#include "EngineManager.h"
#include "Renderer.h"
#include "ComponentStructure.h"
#include "Object.h"
#include "RenderUtils.h"

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

		// Initialization
		if (!pComponent->HasInit())
		{
			pComponent->Init();
			pComponent->MarkAsInit();
		}
		// Render backend
		if (pComponent->ShouldRender())
		{
			EngineManager* pEngineManager = EngineManager::Instance();

			// Has overlapping renderer
			std::vector<RenderType> types = FilterRenderTypesFromVectors(pEngineManager->GetAvailableRenderTypes(), pComponent->GetRenderTypes());
			if (types.size())
			{
				for (RenderType type : types)
				{
					AddRenderType(pComponent, type);
				}
			}
			// Default to current renderer
			else 
			{
				RenderType type = EngineSettings::Instance()->GetRenderType();
				SetRenderType(pComponent, type);
			}
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
				EngineManager* pEngineManager = EngineManager::Instance();

				std::vector<RenderType> types = FilterRenderTypesFromVectors(pEngineManager->GetAvailableRenderTypes(), pComponent->GetRenderTypes());
				for (RenderType type : types)
				{
					RemoveRenderType(pComponent, type);
				}
			}

			pComponent->SetObject(nullptr);
		}
	}
}

void ComponentStructure::SetRenderType(Component* pObject, RenderType type)
{
	// A bit lazy
	ClearRenderTypes(pObject);
	AddRenderType(pObject, type);
}
void ComponentStructure::ClearRenderTypes(Component* pObject)
{
	for (RenderType type : pObject->GetRenderTypes())
	{
		RemoveRenderType(pObject, type);
	}
	pObject->ClearRenderTypes();
}
void ComponentStructure::AddRenderType(Component* pObject, RenderType type)
{
	EngineManager* pEngineManager = EngineManager::Instance();
	AddRenderType(pEngineManager, pObject, type);
}
void ComponentStructure::RemoveRenderType(Component* pObject, RenderType type)
{
	EngineManager* pEngineManager = EngineManager::Instance();
	RemoveRenderType(pEngineManager, pObject, type);
}

void ComponentStructure::AddRenderType(EngineManager* pEngineManager, Component* pObject, RenderType type) 
{
	pObject->AddRenderType(type);
	pEngineManager->GetRenderer(type)->AddRenderable(pObject);
}
void ComponentStructure::RemoveRenderType(EngineManager* pEngineManager, Component* pObject, RenderType type) 
{
	pObject->RemoveRenderType(type);
	pEngineManager->GetRenderer(type)->RemoveRenderable(pObject);
}
