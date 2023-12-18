#include "pch.h"
#include "MaterialParamsMapping.h"

// ---------------------------
// MaterialScalerParamsMapping
// ---------------------------

void MaterialScalarParamsMapping::AddMapping(MaterialScalarParam& newMapping)
{
	m_MaterialScalarParams.push_back(newMapping);

	// Small safety to prevent params from just deleting themselves due to their potentially temporary nature
	newMapping.m_Resource = nullptr;
}
void MaterialScalarParamsMapping::RemoveMapping(MaterialScalarParam& removingMapping)
{
	auto foundMapping = std::find_if(m_MaterialScalarParams.begin(), m_MaterialScalarParams.end(), [removingMapping](const MaterialScalarParam& mapping)
		{
			return removingMapping.m_Name.compare(mapping.m_Name) == 0;
		});
	if (foundMapping != m_MaterialScalarParams.end())
	{
		m_MaterialScalarParams.erase(foundMapping);
	}
}
void MaterialScalarParamsMapping::AddMap(std::vector<MaterialScalarParam>& newMap)
{
	m_MaterialScalarParams.reserve(newMap.size());
	for (MaterialScalarParam& iterator : newMap)
	{
		m_MaterialScalarParams.push_back(iterator);

		// Small safety to prevent params from just deleting themselves due to their potentially temporary nature
		iterator.m_Resource = nullptr;
	}
}
void MaterialScalarParamsMapping::RemoveMap(std::vector<MaterialScalarParam>& removingMap)
{
	for (MaterialScalarParam& iterator : removingMap)
	{
		auto foundMapping = std::find_if(m_MaterialScalarParams.begin(), m_MaterialScalarParams.end(), [iterator](const MaterialScalarParam& mapping)
			{
				return iterator.m_Name.compare(mapping.m_Name) == 0;
			});
		if (foundMapping != m_MaterialScalarParams.end())
		{
			m_MaterialScalarParams.erase(foundMapping);
		}
	}
}

void MaterialScalarParamsMapping::RemoveMappingbasedOnName(std::string& removingName)
{
	auto foundIterator = std::find_if(m_MaterialScalarParams.begin(), m_MaterialScalarParams.end(), [removingName](MaterialScalarParam& iterator)
		{
			return iterator.m_Name.compare(removingName) == 0;
		});
	if (foundIterator != m_MaterialScalarParams.end()) 
	{
		m_MaterialScalarParams.erase(foundIterator);
	}
}
void MaterialScalarParamsMapping::RemoveMapbasedOnName(std::vector<std::string>& removingNaming)
{
	for (std::string& iterator : removingNaming)
	{
		auto foundIterator = std::find_if(m_MaterialScalarParams.begin(), m_MaterialScalarParams.end(), [iterator](const MaterialScalarParam& mapping)
			{
				return mapping.m_Name.compare(iterator) == 0;
			});
		if (foundIterator != m_MaterialScalarParams.end())
		{
			m_MaterialScalarParams.erase(foundIterator);
		}
	}
}

MaterialScalarParam* MaterialScalarParamsMapping::GetMappingbasedOnName(const std::string& findName)
{
	auto foundIterator = std::find_if(m_MaterialScalarParams.begin(), m_MaterialScalarParams.end(), [findName](MaterialScalarParam& iterator)
		{
			return iterator.m_Name.compare(findName) == 0;
		});
	if (foundIterator != m_MaterialScalarParams.end())
	{
		return &(*foundIterator);
	}
	return nullptr;
}

// ---------------------------
// MaterialVectorParamsMapping
// ---------------------------

void MaterialVectorParamsMapping::AddMapping(MaterialVectorParam& newMapping)
{
	m_MaterialVectorParams.push_back(newMapping);
}
void MaterialVectorParamsMapping::RemoveMapping(MaterialVectorParam& removingMapping)
{
	auto foundMapping = std::find_if(m_MaterialVectorParams.begin(), m_MaterialVectorParams.end(), [removingMapping](const MaterialVectorParam& mapping)
		{
			return mapping.m_Name.compare(removingMapping.m_Name) == 0;
		});
	if (foundMapping != m_MaterialVectorParams.end())
	{
		m_MaterialVectorParams.erase(foundMapping);
	}
}
void MaterialVectorParamsMapping::AddMap(std::vector<MaterialVectorParam>& newMap)
{
	m_MaterialVectorParams.reserve(newMap.size());
	for (MaterialVectorParam& iterator : newMap)
	{
		m_MaterialVectorParams.push_back(iterator);
	}
}
void MaterialVectorParamsMapping::RemoveMap(std::vector<MaterialVectorParam>& removingMap)
{
	for (MaterialVectorParam& iterator : removingMap)
	{
		auto foundMapping = std::find_if(m_MaterialVectorParams.begin(), m_MaterialVectorParams.end(), [iterator](const MaterialVectorParam& mapping)
			{
				return mapping.m_Name.compare(iterator.m_Name) == 0;
			});
		if (foundMapping != m_MaterialVectorParams.end())
		{
			m_MaterialVectorParams.erase(foundMapping);
		}
	}
}

void MaterialVectorParamsMapping::RemoveMappingbasedOnName(std::string& removingName)
{
	auto foundIterator = std::find_if(m_MaterialVectorParams.begin(), m_MaterialVectorParams.end(), [removingName](MaterialVectorParam& iterator)
		{
			return iterator.m_Name.compare(removingName) == 0;
		});
	if (foundIterator != m_MaterialVectorParams.end())
	{
		m_MaterialVectorParams.erase(foundIterator);
	}
}
void MaterialVectorParamsMapping::RemoveMapbasedOnName(std::vector<std::string>& removingNaming)
{
	for (std::string& iterator : removingNaming)
	{
		auto foundIterator = std::find_if(m_MaterialVectorParams.begin(), m_MaterialVectorParams.end(), [iterator](const MaterialVectorParam& mapping)
			{
				return mapping.m_Name.compare(iterator) == 0;
			});
		if (foundIterator != m_MaterialVectorParams.end())
		{
			m_MaterialVectorParams.erase(foundIterator);
		}
	}
}

MaterialVectorParam* MaterialVectorParamsMapping::GetMappingbasedOnName(const std::string& findName)
{
	auto foundIterator = std::find_if(m_MaterialVectorParams.begin(), m_MaterialVectorParams.end(), [findName](MaterialVectorParam& iterator)
		{
			return iterator.m_Name.compare(findName) == 0;
		});
	if (foundIterator != m_MaterialVectorParams.end())
	{
		return &(*foundIterator);
	}
	return nullptr;
}

// ----------------------------
// MaterialTextureParamsMapping
// ----------------------------

void MaterialTextureParamsMapping::AddMapping(MaterialTextureParam& newMapping)
{
	m_MaterialTextureParams.push_back(newMapping);

	// Small safety to prevent params from just deleting themselves due to their potentially temporary nature
	newMapping.SetTexture(nullptr, false);
	newMapping.m_Resource = nullptr;
}
void MaterialTextureParamsMapping::RemoveMapping(MaterialTextureParam& removingMapping)
{
	auto foundMapping = std::find_if(m_MaterialTextureParams.begin(), m_MaterialTextureParams.end(), [removingMapping](const MaterialTextureParam& mapping)
		{
			return mapping.m_Name.compare(removingMapping.m_Name) == 0;
		});
	if (foundMapping != m_MaterialTextureParams.end())
	{
		m_MaterialTextureParams.erase(foundMapping);
	}
}
void MaterialTextureParamsMapping::AddMap(std::vector<MaterialTextureParam>& newMap)
{
	m_MaterialTextureParams.reserve(newMap.size());
	for (MaterialTextureParam& iterator : newMap)
	{
		m_MaterialTextureParams.push_back(iterator);

		// Small safety to prevent params from just deleting themselves due to their potentially temporary nature
		iterator.SetTexture(nullptr, false);
		iterator.m_Resource = nullptr;
	}
}
void MaterialTextureParamsMapping::RemoveMap(std::vector<MaterialTextureParam>& removingMap)
{
	for (MaterialTextureParam& iterator : removingMap)
	{
		auto foundMapping = std::find_if(m_MaterialTextureParams.begin(), m_MaterialTextureParams.end(), [iterator](const MaterialTextureParam& mapping)
			{
				return mapping.m_Name.compare(iterator.m_Name) == 0;
			});
		if (foundMapping != m_MaterialTextureParams.end())
		{
			m_MaterialTextureParams.erase(foundMapping);
		}
	}
}

void MaterialTextureParamsMapping::RemoveMappingbasedOnName(std::string& removingName)
{
	auto foundIterator = std::find_if(m_MaterialTextureParams.begin(), m_MaterialTextureParams.end(), [removingName](MaterialTextureParam& iterator)
		{
			return iterator.m_Name.compare(removingName) == 0;
		});
	if (foundIterator != m_MaterialTextureParams.end())
	{
		m_MaterialTextureParams.erase(foundIterator);
	}
}
void MaterialTextureParamsMapping::RemoveMapbasedOnName(std::vector<std::string>& removingNaming)
{
	for (std::string& iterator : removingNaming)
	{
		auto foundIterator = std::find_if(m_MaterialTextureParams.begin(), m_MaterialTextureParams.end(), [iterator](const MaterialTextureParam& mapping)
			{
				return mapping.m_Name.compare(iterator) == 0;
			});
		if (foundIterator != m_MaterialTextureParams.end())
		{
			m_MaterialTextureParams.erase(foundIterator);
		}
	}
}

MaterialTextureParam* MaterialTextureParamsMapping::GetMappingbasedOnName(const std::string& findName)
{
	auto foundIterator = std::find_if(m_MaterialTextureParams.begin(), m_MaterialTextureParams.end(), [findName](MaterialTextureParam& iterator)
		{
			return iterator.m_Name.compare(findName) == 0;
		});
	if (foundIterator != m_MaterialTextureParams.end())
	{
		return &(*foundIterator);
	}
	return nullptr;
}

// --------------------------------
// MaterialEnvironmentParamsMapping
// --------------------------------

void MaterialEnvironmentParamsMapping::AddMapping(D3D_SHADER_MACRO& newMapping)
{
	m_MaterialEnvironmentParams.insert(m_MaterialEnvironmentParams.end() - 1, newMapping);
}
void MaterialEnvironmentParamsMapping::RemoveMapping(D3D_SHADER_MACRO& removingMapping)
{
	auto foundMapping = std::find_if(m_MaterialEnvironmentParams.begin(), m_MaterialEnvironmentParams.end(), [removingMapping](const D3D_SHADER_MACRO& mapping)
		{
			return std::string(mapping.Name).compare(removingMapping.Name) == 0;
		});
	if (foundMapping != m_MaterialEnvironmentParams.end())
	{
		m_MaterialEnvironmentParams.erase(foundMapping);
	}
}
void MaterialEnvironmentParamsMapping::AddMap(std::vector<D3D_SHADER_MACRO>& newMap)
{
	m_MaterialEnvironmentParams.reserve(newMap.size());
	for (D3D_SHADER_MACRO& iterator : newMap)
	{
		m_MaterialEnvironmentParams.push_back(iterator);
	}
}
void MaterialEnvironmentParamsMapping::RemoveMap(std::vector<D3D_SHADER_MACRO>& removingMap)
{
	for (D3D_SHADER_MACRO& iterator : removingMap)
	{
		auto foundMapping = std::find_if(m_MaterialEnvironmentParams.begin(), m_MaterialEnvironmentParams.end(), [iterator](const D3D_SHADER_MACRO& mapping)
			{
				return std::string(mapping.Name).compare(iterator.Name) == 0;
			});
		if (foundMapping != m_MaterialEnvironmentParams.end())
		{
			m_MaterialEnvironmentParams.erase(foundMapping);
		}
	}
}

void MaterialEnvironmentParamsMapping::RemoveMappingbasedOnName(std::string& removingName) 
{
	auto foundIterator = std::find_if(m_MaterialEnvironmentParams.begin(), m_MaterialEnvironmentParams.end(), [removingName](D3D_SHADER_MACRO& iterator)
		{
			return std::string(iterator.Name).compare(removingName) == 0;
		});
	if (foundIterator != m_MaterialEnvironmentParams.end())
	{
		m_MaterialEnvironmentParams.erase(foundIterator);
	}
}
void MaterialEnvironmentParamsMapping::RemoveMapbasedOnName(std::vector<std::string>& removingNaming) 
{
	for (std::string& iterator : removingNaming)
	{
		auto foundIterator = std::find_if(m_MaterialEnvironmentParams.begin(), m_MaterialEnvironmentParams.end(), [iterator](const D3D_SHADER_MACRO& mapping)
			{
				return std::string(mapping.Name).compare(iterator) == 0;
			});
		if (foundIterator != m_MaterialEnvironmentParams.end())
		{
			m_MaterialEnvironmentParams.erase(foundIterator);
		}
	}
}

D3D_SHADER_MACRO* MaterialEnvironmentParamsMapping::GetMappingbasedOnName(const std::string& findName)
{
	auto foundIterator = std::find_if(m_MaterialEnvironmentParams.begin(), m_MaterialEnvironmentParams.end(), [findName](D3D_SHADER_MACRO& iterator)
		{
			return std::string(iterator.Name).compare(findName) == 0;
		});
	if (foundIterator != m_MaterialEnvironmentParams.end())
	{
		return &(*foundIterator);
	}
	return nullptr;
}
