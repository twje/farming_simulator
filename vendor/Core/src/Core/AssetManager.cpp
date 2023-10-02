#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/CommonAssetLoaders.h"
#include "Core/Animation.h"

AssetManager::AssetManager()
{
	// Register common loaders
	RegisterLoader<sf::Texture>(std::make_unique<TextureLoader>());
	RegisterLoader<Animation>(std::make_unique<AnimationLoader>());
}