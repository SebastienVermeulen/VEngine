#include "pch.h"
#include "ShadowData.h"
#include "ShaderStructures.h"

void ShadowData::MarkAsUsed()
{
	for (DepthStencil* depthStencil : m_DepthStencils)
	{
		depthStencil->MarkAsUsed();
	}
}
