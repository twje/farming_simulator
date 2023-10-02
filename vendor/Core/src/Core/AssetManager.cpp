#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/TextureLoader.h"
#include "Core/AnimationLoader.h"

AssetManager::AssetManager()
{
	// Register common loaders
	RegisterLoader<sf::Texture>(std::make_unique<TextureLoader2>());
	RegisterLoader<Animation>(std::make_unique<AnimationLoader2>());
}