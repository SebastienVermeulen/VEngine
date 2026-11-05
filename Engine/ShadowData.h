#pragma once

class DepthStencil;

struct ShadowData
{
	bool IsEmpty() { return m_DepthStencils.empty(); }

	void Add(DepthStencil* stencil) { m_DepthStencils.push_back(stencil); }

	void Clear() { m_DepthStencils.clear(); }

	void MarkAsUsed();

	std::vector<DepthStencil*> m_DepthStencils;
};
