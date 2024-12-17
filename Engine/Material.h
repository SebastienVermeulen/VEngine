#pragma once
#include "ShaderStructures.h"
#include "Texture.h"
#include "MaterialParamsMapping.h" 

class MaterialWidget;
class EngineDevice;
class Light;
class MeshAsset;

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
	void AlignEnvironmentToGeometry(MeshAsset* pAsset);

	void Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr);
	void UpdateMatrix(MatrixTransformationContainer buffer);
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
	inline MaterialTextureParamsMapping& GetTextureParams()
	{
		return m_MaterialTextureParamsMapping;
	}
	inline MaterialScalarParamsMapping& GetScalarParams()
	{
		return m_MaterialScalarParamsMapping;
	}
	inline MaterialVectorParamsMapping& GetVectorParams()
	{
		return m_MaterialVectorParamsMapping;
	}
	inline MaterialEnvironmentParamsMapping& GetMaterialEnvironmentParams()
	{
		return m_MaterialEnvironmentParamsMapping;
	}

private:
	HRESULT InitEffect(ID3D11Device* pDevice);
	HRESULT InitInputLayout(ID3D11Device* pDevice);
	HRESULT InitShaderVariables(ID3D11Device* pDevice);

	// TO-DO: Lot's these member-variables are too specific to have in each material
	//UI
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

	ID3D11Buffer* m_LightsBuffer;
	ID3DX11EffectConstantBuffer* m_LightsBufferVariable;

	//Mappings
	MaterialScalarParamsMapping m_MaterialScalarParamsMapping;
	MaterialVectorParamsMapping m_MaterialVectorParamsMapping;
	MaterialTextureParamsMapping m_MaterialTextureParamsMapping;
	MaterialEnvironmentParamsMapping m_MaterialEnvironmentParamsMapping;

	//Layout
	std::vector<ID3D11InputLayout*> m_pLayouts;
	unsigned int m_pInputLayoutSize;
	unsigned int m_pNrBasisRenderTargets;

	RenderType m_RenderType;

	bool m_Initialized;
};
