#include <iostream>

#include "Core/IApplicationListener.h"
#include "Core/AssetManager.h"
#include "Core/Texture.h"
#include "Core/Animation/Animation.h"
#include "Core/Spritesheet.h"
#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/Animation/AnimationUtils.h"
#include "Level.h"

class Game : public IApplicationListener
{
public:
	void Create() override
	{
		AssetManager& assetManager = GetResourceLocator().GetAssetManager();

		assetManager.LoadAssetsFromManifest<Texture>("../../config/textures.cfg");
		assetManager.LoadAssetsFromManifest<Spritesheet>("../../config/spritesheet.cfg");
		assetManager.LoadAssetsFromManifest<Animation>("../../config/animations.cfg");		
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