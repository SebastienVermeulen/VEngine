#pragma once

class EngineDevice;
class Material;
class Widget;

class GreyScalePostProcess
{
public:
	GreyScalePostProcess();
	virtual ~GreyScalePostProcess();

	GreyScalePostProcess(GreyScalePostProcess& other) = delete;
	GreyScalePostProcess(GreyScalePostProcess&& other) = delete;
	GreyScalePostProcess operator=(GreyScalePostProcess& other) = delete;
	GreyScalePostProcess& operator=(GreyScalePostProcess&& other) = delete;

	bool InitShader(const EngineDevice* pDevice);
	void ReleaseShader();

	void SetupEffectEnvironment(EngineDevice* pDevice);
	void RunEffect(EngineDevice* pDevice);
	void TeardownEffectEnvironment(EngineDevice* pDevice);

	inline bool IsActive() const { return m_bActive; }
	inline void SetActive(const bool bActivate) { m_bActive = bActivate; }

	inline Material* GetMaterial() const { return m_pMaterial; }
	inline Widget* GetGreyScalePostProcessWidget() { return m_pGreyScalePostProcessWidget; }

private:
	// TO-DO: Make a permanent reusable one
	ID3D11Buffer* m_pScreenQuadVertexBuffer, * m_pScreenQuadIndexBuffer;
	UINT m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies;

	Material* m_pMaterial;

	bool m_bActive;

	Widget* m_pGreyScalePostProcessWidget;
};
