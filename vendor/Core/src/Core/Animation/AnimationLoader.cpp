#include "Core/Animation/AnimationLoader.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"

// --------------------------------------------------------------------------------
/*virtual*/ std::unique_ptr<Asset> AnimationLoader::Load(AssetDescriptor<Animation> descriptor)
{
	return Animation::LoadFromFile(descriptor.GetFilePath());
}