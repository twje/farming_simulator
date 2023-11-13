#include <SFML/Graphics.hpp>

#include "Core/CommonAssetLoaders.h"
#include "Core/Texture.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"
#include "Core/Spritesheet.h"

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> TextureLoader::Load(const std::string& fileName)
{
	sf::Texture texture;
	if (!texture.loadFromFile(fileName))
	{
		throw std::runtime_error("Failed to load texture: " + fileName);
	}
	return std::make_unique<Texture>(std::move(texture));
}

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> SpritesheetLoader::Load(const std::string& filePath)
{
	return Spritesheet::LoadFromFile(filePath);		
}