#pragma once

#include <Core/ResourceManager.h>
#include <Core/Animation.h>

class AnimationLoader : public ResourceLoader<Animation>
{
public:
	std::unique_ptr<Animation> Load(std::string filePath, ResourceLocator& locator) override;
};

class AnimationManager : public ResourceManager<Animation>
{
public:
	AnimationManager(ResourceLocator& locator);
};