#include "Core/AnimationManager.h"

#include <Core/Animation.h>
#include <Core/TextureAnimationSequence.h>
#include <Core/ResourceLocator.h>

std::unique_ptr<Animation> AnimationLoader::Load(std::string filePath, ResourceLocator& locator)
{	
    std::initializer_list<std::string> frameList = {
        "character_up_0",
        "character_up_1",
        "character_up_2",
        "character_up_3"
    };
    auto sequence = std::make_shared<TextureAnimationSequence>("up", 8, locator.GetTextureManager(), frameList);
	
    auto animation = std::make_unique<Animation>();
    animation->AddAnimationSequence(sequence);
	
    return animation;
}

AnimationManager::AnimationManager(ResourceLocator& locator)
	: ResourceManager<Animation>(std::make_unique<AnimationLoader>(), locator)
{ }