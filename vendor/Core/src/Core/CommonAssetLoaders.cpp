#include "Core/CommonAssetLoaders.h"

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/Texture.h"
#include "Core/Spritesheet.h"

// Third party
#include <SFML/Graphics.hpp>

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> TextureLoader::Load(AssetFileDescriptor<Texture> descriptor)
{
	return Load(descriptor.GetFilePath());
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> TextureLoader::Load(AssetMemoryDescriptor<Texture> descriptor)
{
	const YAML::Node& data = descriptor.GetData();
	const std::string& filePath = data["filePath"].as<std::string>();
	return Load(filePath);
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> TextureLoader::Load(const std::string& filePath)
{
	sf::Texture texture;
	if (!texture.loadFromFile(filePath))
	{
		throw std::runtime_error("Failed to load texture: " + filePath);
	}	
	return std::make_unique<Texture>(std::move(texture));
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> SpritesheetLoader::Load(AssetFileDescriptor<Spritesheet> descriptor)
{
	return Spritesheet::LoadFromFile(descriptor.GetFilePath());
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> SpritesheetLoader::Load(AssetMemoryDescriptor<Spritesheet> descriptor)
{	
	return Spritesheet::Deserialize(descriptor.GetData());
}