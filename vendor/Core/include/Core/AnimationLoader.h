#pragma once

#include <memory>

#include "Core/AssetManager.h"
#include "Core/Animation.h"

class AnimationLoader2 : public AssetLoader
{
public:
	virtual std::unique_ptr<AssetBase> Load(const std::string& fileName, AssetManager& assetManager) override
	{
		return nullptr;
	}
};