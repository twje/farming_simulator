#include <iostream>

#include "Core/IApplicationListener.h"
#include "Core/AssetManager.h"
#include "Core/Texture.h"
#include "Core/Animation/Animation.h"
#include "Core/Shader.h"
#include "Core/Spritesheet.h"
#include "Core/Tiled/TiledMap.h"
#include "Level.h"

class Game : public IApplicationListener
{
public:
	void Create() override
	{
		AssetManager& assetManager = GetResourceLocator().GetAssetManager();

		// Register Loaders
		assetManager.RegisterLoader<TiledMap>(std::make_unique<TiledMapLoader>());
		assetManager.RegisterLoader<Shader>(std::make_unique<ShaderLoader>());

		// Load descriptors
		assetManager.LoadAssetsFromManifest<Texture>("../../config/textures.cfg");
		assetManager.LoadAssetsFromManifest<Spritesheet>("../../config/spritesheet.cfg");
		assetManager.LoadAssetsFromManifest<Animation>("../../config/animations.cfg");
		assetManager.LoadAssetsFromManifest<TiledMap>("../../config/maps.cfg");
		assetManager.LoadAssetsFromManifest<Shader>("../../config/shaders.cfg");
		assetManager.ProcessAssetQueue();

		PushLayer(std::make_unique<Level>());
	}

private:
	AssetManager mAssetManager;
};

std::unique_ptr<IApplicationListener> CreateApplication()
{
	return std::make_unique<Game>();
}