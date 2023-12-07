#pragma once

#include "Core/AssetManager.h"

class TiledMapAsset;

class TiledMapLoader : public AssetLoader<TiledMapAsset>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<TiledMapAsset> descriptor) override;
};