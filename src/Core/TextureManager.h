#pragma once

#include <SFML/Graphics.hpp>

#include <Core/ResourceManager.h>

class TextureLoader : public ResourceLoader<sf::Texture>
{
public:
	std::unique_ptr<sf::Texture> Load(std::string filePath) override
	{
		auto texture = std::make_unique<sf::Texture>();
		if (!texture->loadFromFile(filePath))
		{
		    throw std::runtime_error("Failed to load texture: " + filePath);
		}
		return texture;
	}
};

class TextureManager : public ResourceManager<sf::Texture>
{
public:
	TextureManager()
		: ResourceManager<sf::Texture>(std::make_unique<TextureLoader>())
	{}
};