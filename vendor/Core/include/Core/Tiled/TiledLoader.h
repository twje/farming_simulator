#pragma once

#include "Core/AssetManager.h"

class TiledMap;

class TiledMapLoader : public AssetLoader<TiledMap>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<TiledMap> descriptor) override;
};