#include "Core/CommonAssetLoaders.h"

#include <SFML/Graphics.hpp>

#include "Core/Texture.h"
#include "Core/Spritesheet.h"

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> TextureLoader::Load(AssetDescriptor<Texture> descriptor)
{
	sf::Texture texture;
	if (!texture.loadFromFile(descriptor.GetFilePath()))
	{
		throw std::runtime_error("Failed to load texture: " + descriptor.GetFilePath());
	}
	return std::make_unique<Texture>(std::move(texture));
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> SpritesheetLoader::Load(AssetDescriptor<Spritesheet> descriptor)
{
	return Spritesheet::LoadFromFile(descriptor.GetFilePath());
}