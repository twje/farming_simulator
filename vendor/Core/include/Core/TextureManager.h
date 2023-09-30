#pragma once

#include <SFML/Graphics.hpp>

#include <Core/ResourceManager.h>

class TextureLoader : public ResourceLoader<sf::Texture>
{
public:
	std::unique_ptr<sf::Texture> Load(std::string filePath, ResourceLocator& locator) override;
};

class TextureManager : public ResourceManager<sf::Texture>
{
public:
	TextureManager(ResourceLocator& locator);
};