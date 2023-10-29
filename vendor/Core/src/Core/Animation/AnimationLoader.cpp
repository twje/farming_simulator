#include "Core/Animation/AnimationLoader.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"

// --------------------------------------------------------------------------------
std::unique_ptr<AssetBase> AnimationLoader::Load(const std::string& filePath, AssetManager& assetManager)
{
	AnimationFactory factory;
	factory.LoadFromFile(filePath);
	auto animation = factory.CreateAnimation(assetManager);
	return std::make_unique<Asset<Animation>>(std::move(animation));
}
