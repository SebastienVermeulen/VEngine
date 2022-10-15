#pragma once
#include "Asset.h"

class AssetFactory abstract
{
public:
	AssetFactory();
	~AssetFactory();

	AssetFactory(AssetFactory& other) = delete;
	AssetFactory(AssetFactory&& other) = delete;
	AssetFactory operator=(AssetFactory& other) = delete;
	AssetFactory& operator=(AssetFactory&& other) = delete;

	void AddAssetToPool(Asset* pAsset);
	void RemoveAssetFromPool(Asset* pAsset);
	void IncrementAssetCounter();
	void DecrementAssetCounter();
	void SafetyLoopAroundOutOfRange();

private:
	static std::vector<Asset*> m_AssetPointerPool;
	static unsigned int m_AssetCounter;
};
