#pragma once

#include <SFML/Graphics.hpp>

#include <Core/ResourceManager.h>
#include <Core/Animation.h>

class AnimationLoader : public ResourceLoader<Animation>
{
public:
	std::unique_ptr<Animation> Load(std::string filePath) override
	{
		return nullptr;
	}
};

class AnimationManager : public ResourceManager<Animation>
{
public:
	AnimationManager()
		: ResourceManager<Animation>(std::make_unique<AnimationLoader>())
	{ }
};