#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/CommonAssetLoaders.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationLoader.h"
#include "Core/Spritesheet.h"

AssetManager::AssetManager()
{
	// Register common loaders
	RegisterLoader<sf::Texture>(std::make_unique<TextureLoader>());
	RegisterLoader<Spritesheet>(std::make_unique<SpritesheetLoader>());
	RegisterLoader<Animation>(std::make_unique<AnimationLoader>());
}