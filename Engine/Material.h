#pragma once
#include "ShaderStructures.h"
#include "Texture.h"

class MaterialWidget;
class EngineDevice;
class Light;

struct MaterialTextureParam 
{
	~MaterialTextureParam()
	{
		SafeRelease(m_Resource);
		SafeDelete(m_pTexture);
	}

	Texture* m_pTexture;
	ID3DX11EffectShaderResourceVariable* m_Resource;
	std::string m_Name;
	std::wstring m_LocalFilePath;
};
struct MaterialScalarParam
{
	~MaterialScalarParam()
	{
		SafeRelease(m_Resource);
	}

	ID3DX11EffectScalarVariable* m_Resource;
	float m_Value;
	float m_Min;
	float m_Max;
	std::string m_Name;
};

class Material 
{
public:
	Material(const std::wstring& effectFile, RenderType type = RenderType::deferred);
	virtual ~Material();

	Material(Material& other) = delete;
	Material(Material&& other) = delete;
	Material operator=(Material& other) = delete;
	Material& operator=(Material&& other) = delete;

	HRESULT InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr);
	void UpdateMatrix(MatrixRenderBuffer buffer);
	void UpdateMaterialLighting(ID3D11DeviceContext* pContext, std::vector<ShaderLight> lights);
	HRESULT UpdateParameterValues(EngineDevice* pEngineDevice);
	HRESULT ExplicitlyUnbindingResources(EngineDevice* pEngineDevice);

	inline void SetRendertype(RenderType renderType) 
	{
		m_RenderType = renderType; 
	}
	inline RenderType GetIfDeferred() const 
	{
		return m_RenderType; 
	}

	MaterialWidget* GetWidget() const;
	inline std::vector<MaterialTextureParam*>& GetTextureParams() 
	{
		return m_MaterialTextureParams;
	}
	inline std::vector<MaterialScalarParam*>& GetScalarParams()
	{
		return m_MaterialScalarParams;
	}

private:
	HRESULT InitEffect(ID3D11Device* pDevice);
	HRESULT InitInputLayout(ID3D11Device* pDevice);
	HRESULT InitShaderVariables(ID3D11Device* pDevice);

	MaterialWidget* m_pWidget;

	//Effect
	std::wstring m_EffectFile;
	ID3DX11Effect* m_pEffect;

	//Technique
	std::wstring m_TechniqueName;
	ID3DX11EffectTechnique* m_pTechnique;

	//Globals
	ID3DX11EffectMatrixVariable* m_pWorld;
	ID3DX11EffectMatrixVariable* m_pView;
	ID3DX11EffectMatrixVariable* m_pInverseView;
	ID3DX11EffectMatrixVariable* m_pProjection;
	ID3DX11EffectMatrixVariable* m_pWorldViewProj;
	std::vector<MaterialTextureParam*> m_MaterialTextureParams;
	std::vector<MaterialScalarParam*> m_MaterialScalarParams;

	ID3D11Buffer* m_LightsBuffer;
	ID3DX11EffectConstantBuffer* m_BufferVariable;

	//Layout
	std::vector<ID3D11InputLayout*> m_pLayouts;
	unsigned int m_pInputLayoutSize;
	unsigned int m_pNrBasisRenderTargets;

	RenderType m_RenderType;
};
