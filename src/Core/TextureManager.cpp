#include "Core/TextureManager.h"
#include "Core/ResourceLocator.h"

std::unique_ptr<sf::Texture> TextureLoader::Load(std::string filePath, ResourceLocator& locator)
{
	auto texture = std::make_unique<sf::Texture>();
	if (!texture->loadFromFile(filePath))
	{
		throw std::runtime_error("Failed to load texture: " + filePath);
	}
	return texture;
}

TextureManager::TextureManager(ResourceLocator& locator)
	: ResourceManager<sf::Texture>(std::make_unique<TextureLoader>(), locator)
{ }