#include "pch.h"
#include "AssetFactory.h"

unsigned int AssetFactory::m_AssetCounter = 0;
std::vector<Asset*> AssetFactory::m_AssetPointerPool = std::vector<Asset*>{};

AssetFactory::AssetFactory()
{
}
AssetFactory::~AssetFactory()
{
}

//Used for keeping track of the number of assets should normally not go out of range, but if it does the safety reorders
void AssetFactory::AddAssetToPool(Asset* pAsset)
{
	if (std::find(m_AssetPointerPool.begin(), m_AssetPointerPool.end(), pAsset) == m_AssetPointerPool.end())
	{
		m_AssetPointerPool.push_back(pAsset);
	}
}
void AssetFactory::RemoveAssetFromPool(Asset* pAsset)
{
	if (m_AssetPointerPool.size())
	{
		typename std::vector<Asset*>::iterator it = std::find(m_AssetPointerPool.begin(), m_AssetPointerPool.end(), pAsset);
		if (it != m_AssetPointerPool.end())
		{
			m_AssetPointerPool.erase(it);
		}
	}
}
void AssetFactory::IncrementAssetCounter()
{
	if (UINT_MAX == m_AssetCounter)
	{
		m_AssetCounter = 0;
		SafetyLoopAroundOutOfRange();
		return;
	}
	++m_AssetCounter;
}
void AssetFactory::DecrementAssetCounter()
{
	if (0 == m_AssetCounter) 
	{
		return;
	}
	--m_AssetCounter;
}
void AssetFactory::SafetyLoopAroundOutOfRange()
{
}
