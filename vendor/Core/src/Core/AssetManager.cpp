#include <SFML/Graphics.hpp>

#include "Core/Texture.h"
#include "Core/AssetManager.h"
#include "Core/CommonAssetLoaders.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationLoader.h"
#include "Core/Spritesheet.h"
#include "Core/Tiled/TiledMap.h"
#include "Core/Tiled/TiledLoader.h"

AssetManager::AssetManager()
{
	// Register common loaders
	RegisterLoader<Texture>(std::make_unique<TextureLoader>());
	RegisterLoader<Spritesheet>(std::make_unique<SpritesheetLoader>());
	RegisterLoader<Animation>(std::make_unique<AnimationLoader>());
	RegisterLoader<TiledMap>(std::make_unique<TiledMapLoader>());
}