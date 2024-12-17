#pragma once

class Material;

// TO-DO: Inherit from material instead
class ShadowCasting
{
public:
	ShadowCasting();
	virtual ~ShadowCasting();

	ShadowCasting(ShadowCasting& other) = delete;
	ShadowCasting(ShadowCasting&& other) = delete;
	ShadowCasting operator=(ShadowCasting& other) = delete;
	ShadowCasting& operator=(ShadowCasting&& other) = delete;

	void Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr);

	Material* GetShadowMaterial();

private:
	Material* m_pMaterial;
};
