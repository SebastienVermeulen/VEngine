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
	float m_Min;
	float m_Max;
	std::string m_Name;
};

// =================
// Material Mappings
// =================

struct MaterialScalarParamsMapping
{
	MaterialScalarParamsMapping();
	~MaterialScalarParamsMapping();

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

	MaterialScalarParam* GetMappingbasedOnName(std::string& findName);

	inline void Empty() { m_MaterialScalarParams.clear(); }

	std::vector<MaterialScalarParam> m_MaterialScalarParams;
};

struct MaterialTextureParamsMapping
{
	MaterialTextureParamsMapping();
	~MaterialTextureParamsMapping();

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