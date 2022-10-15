#include "pch.h"
#include "MeshFactory.h"
#include "MeshAsset.h"
#include "FileManager.h"

MeshFactory::MeshFactory() 
	:AssetFactory()
	, Singleton<MeshFactory>()
{
}
MeshFactory::~MeshFactory() 
{
}

MeshAsset* MeshFactory::CreateAsset(const std::wstring& localFilePath)
{
	MeshAsset* pAsset = new MeshAsset();

	//TO-DO: Make an asset reference holder, to look up already parsed meshes (think UAsset)
	pAsset->SetFilePath(localFilePath);
	
	if (FAILED(FileManager::LoadFBX(localFilePath, pAsset)))
	{
		//TO-DO: Make logger, preloading the FBX failed
	}
	return pAsset;
}
