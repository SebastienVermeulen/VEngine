#include "pch.h"
#include "ObjectStructure.h"
#include "Object.h"

ObjectStructure::ObjectStructure(Object* pObject)
	:m_pParent{ nullptr }
	, m_pChildInstance{ pObject }
	, m_Objects{}
{
}
ObjectStructure::~ObjectStructure()
{
	m_pParent = nullptr;
	SafeDelete(m_Objects);
}

Object* ObjectStructure::GetParent() const
{ 
	return m_pParent; 
}
void ObjectStructure::SetParent(Object* pObject)
{ 
	m_pParent = pObject; 
}

void ObjectStructure::AttachObject(Object* pObject)
{
	if (m_Objects.size())
	{
		if (std::find(m_Objects.begin(), m_Objects.end(), pObject) == m_Objects.end())
		{
			m_Objects.push_back(pObject);
			pObject->SetParent(m_pChildInstance);
		}
	}
}
void ObjectStructure::AttachObjectTo(Object* pObject)
{
	pObject->AttachObject(m_pChildInstance);
}
void ObjectStructure::DetachObject(Object* pObject)
{
	if (m_Objects.size())
	{
		std::vector<Object*>::iterator it = std::find(m_Objects.begin(), m_Objects.end(), pObject);
		if (it != m_Objects.end())
		{
			m_Objects.erase(it);
			pObject->SetParent(nullptr);
		}
	}
}
void ObjectStructure::DetachObjectFrom()
{
	m_pParent->DetachObject(m_pChildInstance);
}
