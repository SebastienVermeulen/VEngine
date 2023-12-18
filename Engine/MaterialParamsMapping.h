#pragma once
#include "Texture.h"
#include "ShaderStructures.h"

// ===============
// Material Params
// ===============

struct MaterialTextureParam
{
public:
	MaterialTextureParam(const bool holdsRendertarget, const  std::string& name, const std::wstring& localFilePath)
	{
		m_Resource = nullptr;
		m_HoldsRendertarget = holdsRendertarget;
		m_Name = name;
		m_LocalFilePath = localFilePath;
		m_pTexture = nullptr;
	}
	MaterialTextureParam(ID3DX11EffectShaderResourceVariable* pResource, const bool holdsRendertarget, const std::string& name, const std::wstring& localFilePath, Texture* pTexture)
	{
		m_Resource = pResource;
		m_HoldsRendertarget = holdsRendertarget;
		m_Name = name;
		m_LocalFilePath = localFilePath;
		m_pTexture = pTexture;
	}
	~MaterialTextureParam()
	{
		SafeRelease(m_Resource);
		if (!m_HoldsRendertarget)
		{
			SafeDelete(m_pTexture);
		}
	}

	Texture* GetTexture() 
	{
		return m_pTexture;
	}
	void SetTexture(Texture* pNewTexture, bool holdsRendertarget)
	{
		m_pTexture = pNewTexture;
		m_HoldsRendertarget = holdsRendertarget;
	}

	ID3DX11EffectShaderResourceVariable* m_Resource;
	bool m_HoldsRendertarget = false;
	
	std::string m_Name;
	std::wstring m_LocalFilePath;
	
private:
	Texture* m_pTexture;
};

struct MaterialScalarParam
{
	~MaterialScalarParam()
	{
		SafeRelease(m_Resource);
	}

	ID3DX11EffectScalarVariable* m_Resource;
	float m_Value;
	float m_Min = 0.0f;
	float m_Max = 0.0f;
	std::string m_Name;
};

struct MaterialVectorParam
{
	~MaterialVectorParam()
	{
		SafeRelease(m_Resource);
	}

	ID3DX11EffectVectorVariable* m_Resource;
	DirectX::XMFLOAT4 m_Value;
	float m_Min = 0.0f;
	float m_Max = 0.0f;
	std::string m_Name;
};

// =================
// Material Mappings
// =================

struct MaterialScalarParamsMapping
{
	MaterialScalarParamsMapping() {}
	~MaterialScalarParamsMapping() {}

	MaterialScalarParamsMapping(MaterialScalarParamsMapping& other) = delete;
	MaterialScalarParamsMapping(MaterialScalarParamsMapping&& other) = delete;
	MaterialScalarParamsMapping operator=(MaterialScalarParamsMapping& other) = delete;
	MaterialScalarParamsMapping& operator=(MaterialScalarParamsMapping&& other) = delete;

	void AddMapping(MaterialScalarParam& newMapping);
	void RemoveMapping(MaterialScalarParam& removingMapping);
	void AddMap(std::vector<MaterialScalarParam>& newMap);
	void RemoveMap(std::vector<MaterialScalarParam>& removingMap);

	void RemoveMappingbasedOnName(std::string& removingName);
	void RemoveMapbasedOnName(std::vector<std::string>& removingNaming);

	MaterialScalarParam* GetMappingbasedOnName(const std::string& findName);

	inline void Empty() { m_MaterialScalarParams.clear(); }

	std::vector<MaterialScalarParam> m_MaterialScalarParams;
};

struct MaterialVectorParamsMapping
{
	MaterialVectorParamsMapping() {}
	~MaterialVectorParamsMapping() {}

	MaterialVectorParamsMapping(MaterialVectorParamsMapping& other) = delete;
	MaterialVectorParamsMapping(MaterialVectorParamsMapping&& other) = delete;
	MaterialVectorParamsMapping operator=(MaterialVectorParamsMapping& other) = delete;
	MaterialVectorParamsMapping& operator=(MaterialVectorParamsMapping&& other) = delete;

	void AddMapping(MaterialVectorParam& newMapping);
	void RemoveMapping(MaterialVectorParam& removingMapping);
	void AddMap(std::vector<MaterialVectorParam>& newMap);
	void RemoveMap(std::vector<MaterialVectorParam>& removingMap);

	void RemoveMappingbasedOnName(std::string& removingName);
	void RemoveMapbasedOnName(std::vector<std::string>& removingNaming);

	MaterialVectorParam* GetMappingbasedOnName(const std::string& findName);

	inline void Empty() { m_MaterialVectorParams.clear(); }

	std::vector<MaterialVectorParam> m_MaterialVectorParams;
};

struct MaterialTextureParamsMapping
{
	MaterialTextureParamsMapping() {}
	~MaterialTextureParamsMapping() {}

	MaterialTextureParamsMapping(MaterialTextureParamsMapping& other) = delete;
	MaterialTextureParamsMapping(MaterialTextureParamsMapping&& other) = delete;
	MaterialTextureParamsMapping operator=(MaterialTextureParamsMapping& other) = delete;
	MaterialTextureParamsMapping& operator=(MaterialTextureParamsMapping&& other) = delete;

	void AddMapping(MaterialTextureParam& newMapping);
	void RemoveMapping(MaterialTextureParam& removingMapping);
	void AddMap(std::vector<MaterialTextureParam>& newMap);
	void RemoveMap(std::vector<MaterialTextureParam>& removingMap);

	void RemoveMappingbasedOnName(std::string& removingName);
	void RemoveMapbasedOnName(std::vector<std::string>& removingNaming);

	MaterialTextureParam* GetMappingbasedOnName(const std::string& findName);

	inline void Empty() { m_MaterialTextureParams.clear(); }

	std::vector<MaterialTextureParam> m_MaterialTextureParams;
};

struct MaterialEnvironmentParamsMapping
{
	MaterialEnvironmentParamsMapping() { m_MaterialEnvironmentParams.push_back(D3D_SHADER_MACRO{ NULL, NULL }); }
	~MaterialEnvironmentParamsMapping() {}

	MaterialEnvironmentParamsMapping(MaterialEnvironmentParamsMapping& other) = delete;
	MaterialEnvironmentParamsMapping(MaterialEnvironmentParamsMapping&& other) = delete;
	MaterialEnvironmentParamsMapping operator=(MaterialEnvironmentParamsMapping& other) = delete;
	MaterialEnvironmentParamsMapping& operator=(MaterialEnvironmentParamsMapping&& other) = delete;

	void AddMapping(D3D_SHADER_MACRO& newMapping);
	void RemoveMapping(D3D_SHADER_MACRO& removingMapping);
	void AddMap(std::vector<D3D_SHADER_MACRO>& newMap);
	void RemoveMap(std::vector<D3D_SHADER_MACRO>& removingMap);

	void RemoveMappingbasedOnName(std::string& removingName);
	void RemoveMapbasedOnName(std::vector<std::string>& removingNaming);

	D3D_SHADER_MACRO* GetMappingbasedOnName(const std::string& findName);

	inline void Empty() { m_MaterialEnvironmentParams.clear(); m_MaterialEnvironmentParams.push_back(D3D_SHADER_MACRO{ NULL, NULL }); }

	std::vector<D3D_SHADER_MACRO> m_MaterialEnvironmentParams;
};
