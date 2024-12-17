#include "pch.h"
#include "ShadowCasting.h"
#include "Material.h"

ShadowCasting::ShadowCasting()
	:m_pMaterial{ nullptr }
{
	m_pMaterial = new Material(L"..\\Resources\\Shaders\\ShadowRendering.fx", RenderType::shadowPass);
}
ShadowCasting::~ShadowCasting()
{
	SafeDelete(m_pMaterial);
}

void ShadowCasting::Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr)
{
	m_pMaterial->Render(pContext, nrIndices, passNr);
}

Material* ShadowCasting::GetShadowMaterial()
{
	return m_pMaterial;
}
