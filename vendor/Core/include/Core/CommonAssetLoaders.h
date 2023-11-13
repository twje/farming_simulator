#pragma once

#include <memory>

#include "Core/AssetManager.h"

// --------------------------------------------------------------------------------
class TextureLoader : public AssetLoader
{
public:
	virtual std::unique_ptr<Asset> Load(const std::string& fileName) override;
};

// --------------------------------------------------------------------------------
class SpritesheetLoader : public AssetLoader
{
public:
	virtual std::unique_ptr<Asset> Load(const std::string& filePath) override;
};
