#include "pch.h"
#include "RenderAnnotation.h"
#include "Logger.h"

RenderAnnotation::RenderAnnotation(const std::wstring annotation)
{
	RenderAnnotationsContainer::Instance()->MakeAnnotation(annotation, *this);
}
RenderAnnotation::~RenderAnnotation()
{
	// We went out of scope, time to try and remove the annotation
	if (!m_WasRemoved)
	{
		RenderAnnotationsContainer::Instance()->RemoveAnnotation(*this);
		m_WasRemoved = true;
	}
}

RenderAnnotationsContainer::RenderAnnotationsContainer()
{
}
RenderAnnotationsContainer::~RenderAnnotationsContainer()
{
	// Make sure to clean up annotations
	while (m_Annotations.size()) 
	{
		m_pAnnotationsInterface->EndEvent();
		m_Annotations.pop();
	}
}

HRESULT RenderAnnotationsContainer::Init(ID3D11DeviceContext* pDeviceContext)
{
	return pDeviceContext->QueryInterface(__uuidof(m_pAnnotationsInterface), reinterpret_cast<void**>(&m_pAnnotationsInterface));
}

void RenderAnnotationsContainer::MakeAnnotation(const std::wstring newAnnotation, RenderAnnotation& renderAnnotation)
{
	m_pAnnotationsInterface->BeginEvent(newAnnotation.c_str());
	renderAnnotation.m_ID = std::rand();
	m_Annotations.push(&renderAnnotation);
}
void RenderAnnotationsContainer::RemoveAnnotation(RenderAnnotation& renderAnnotation)
{
	// Lifetime has already passed
	if (renderAnnotation.m_WasRemoved)
	{
		return;
	}

	// Make sure we only remove the top of our stack, to avoid weirdness
	if (m_Annotations.top()->m_ID == renderAnnotation.m_ID)
	{
		renderAnnotation.m_WasRemoved = true;
		m_pAnnotationsInterface->EndEvent();
		m_Annotations.pop();
	}
	else 
	{
		V_LOG(LogVerbosity::Log, V_WTEXT("The annotation you tried to remove wasn't at the top of the stack."));
	}
}
