#pragma once
#include "AssetFactory.h"
#include "Singleton.h"

class MeshAsset;

class MeshFactory : public AssetFactory, public Singleton<MeshFactory>
{
public:
	MeshFactory();
	~MeshFactory();

	MeshFactory(MeshFactory& other) = delete;
	MeshFactory(MeshFactory&& other) = delete;
	MeshFactory operator=(MeshFactory& other) = delete;
	MeshFactory& operator=(MeshFactory&& other) = delete;

	virtual MeshAsset* CreateAsset(const std::wstring& localFilePath);
};
