#include "pch.h"
#include "Asset.h"

Asset::Asset() 
	:m_FilePath{}
	, m_AssetID{ 0 }
{
}
Asset::~Asset()
{
}

void Asset::SetFilePath(const std::wstring& filePath)
{
	m_FilePath = filePath;
}

void Asset::WriteToVAsset() const 
{
}