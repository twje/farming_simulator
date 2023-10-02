#include "Core/AnimationManager.h"

#include <Core/Animation.h>
#include <Core/TextureAnimationSequence.h>
#include <Core/ResourceLocator.h>

std::unique_ptr<Animation> AnimationLoader::Load(std::string filePath, ResourceLocator& locator)
{
    return nullptr;
}

AnimationManager::AnimationManager(ResourceLocator& locator)
	: ResourceManager<Animation>(std::make_unique<AnimationLoader>(), locator)
{ }