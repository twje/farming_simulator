#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"

class TextureLoader2 : public AssetLoader
{
public:
	virtual std::unique_ptr<AssetBase> Load(const std::string& fileName, AssetManager& assetManager) override
	{
		auto texture = std::make_unique<sf::Texture>();
		if (!texture->loadFromFile(fileName))
		{
			throw std::runtime_error("Failed to load texture: " + fileName);
		}
		return std::make_unique<Asset<sf::Texture>>(std::move(texture));
	}
};