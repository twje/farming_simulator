#pragma once

#include "Core/AssetManager.h"
#include "Core/Tiled/TiledMapData.h"
#include "Core/Utils.h"

class TiledMap : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMap(std::unique_ptr<TiledMapData> data)
		: mData(std::move(data))
	{ }

	virtual void ResolveAssetDeps(AssetManager& assetManager)
	{

	}

	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors()
	{		
		return { };
	}

private:
	std::unique_ptr<TiledMapData> mData;
};