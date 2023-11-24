#pragma once

class EngineDevice;
class Material;
class Widget;
class Texture;

class ScreenSpaceCopy
{
public:
	ScreenSpaceCopy();
	virtual ~ScreenSpaceCopy();

	ScreenSpaceCopy(ScreenSpaceCopy& other) = delete;
	ScreenSpaceCopy(ScreenSpaceCopy&& other) = delete;
	ScreenSpaceCopy operator=(ScreenSpaceCopy& other) = delete;
	ScreenSpaceCopy& operator=(ScreenSpaceCopy&& other) = delete;

	bool InitShader(const EngineDevice* pDevice);
	void ReleaseShader();

	void RunEffect(EngineDevice* pDevice, Texture* pSource, Texture* pTarget);

	inline Material* GetMaterial() const { return m_pMaterial; }

private:
	// TO-DO: Make a permanent reusable one
	ID3D11Buffer* m_pScreenQuadVertexBuffer, * m_pScreenQuadIndexBuffer;
	UINT m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies;

	Material* m_pMaterial;
};
