#include "Core/AnimationManager.h"

#include <Core/Animation.h>
#include <Core/TextureAnimationSequence.h>
#include <Core/ResourceLocator.h>

std::unique_ptr<Animation> AnimationLoader::Load(std::string filePath, ResourceLocator& locator)
{	
    auto animation = std::make_unique<Animation>();
    animation->LoadFromFile(filePath, locator);
    return animation;
}

AnimationManager::AnimationManager(ResourceLocator& locator)
	: ResourceManager<Animation>(std::make_unique<AnimationLoader>(), locator)
{ }