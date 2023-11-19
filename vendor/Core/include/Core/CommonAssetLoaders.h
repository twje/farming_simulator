#pragma once

#include <memory>

#include "Core/AssetManager.h"

// Forward Declarations
// --------------------------------------------------------------------------------
class Texture;
class Spritesheet;

// --------------------------------------------------------------------------------
class TextureLoader : public AssetLoader<Texture>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<Texture> descriptor) override;
};

// --------------------------------------------------------------------------------
class SpritesheetLoader : public AssetLoader<Spritesheet>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<Spritesheet> descriptor) override;
};
