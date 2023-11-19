#pragma once

#include "Core/AssetManager.h"

#include <memory>

class Animation;

class AnimationLoader : public AssetLoader<Animation>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetDescriptor<Animation> descriptor) override;
};