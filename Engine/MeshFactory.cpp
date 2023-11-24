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

HRESULT MeshFactory::CreateAsset(const std::wstring& localFilePath, MeshAsset** pMeshAsset)
{
	*pMeshAsset = new MeshAsset();

	//TO-DO: Make an asset reference holder, to look up already parsed meshes (think UAsset)
	(*pMeshAsset)->SetFilePath(localFilePath);
	
	if (FAILED(FileManager::LoadOBJ(localFilePath, *pMeshAsset)))
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("MeshComponent: Failed to load FBX file for %s.", FileManager::GetFileName(localFilePath)));
		return E_FAIL;
	}
	return S_OK;
}
