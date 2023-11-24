#include "pch.h"
#include "MaterialParamsMapping.h"

// ---------------------------
// MaterialScalerParamsMapping
// ---------------------------

MaterialScalarParamsMapping::MaterialScalarParamsMapping()
{
}
MaterialScalarParamsMapping::~MaterialScalarParamsMapping()
{
}

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

MaterialScalarParam* MaterialScalarParamsMapping::GetMappingbasedOnName(std::string& findName)
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

// ----------------------------
// MaterialTextureParamsMapping
// ----------------------------

MaterialTextureParamsMapping::MaterialTextureParamsMapping()
{
}
MaterialTextureParamsMapping::~MaterialTextureParamsMapping()
{
}

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
