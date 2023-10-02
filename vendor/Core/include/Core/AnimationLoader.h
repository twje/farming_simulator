#pragma once

#include <memory>

#include "Core/AssetManager.h"
#include "Core/Animation.h"

class AnimationLoader2 : public AssetLoader
{
public:
	virtual std::unique_ptr<AssetBase> Load(const std::string& filePath, AssetManager& assetManager) override
	{
		auto animation = std::make_unique<Animation>();
		animation->LoadFromFile(filePath, assetManager);
		return std::make_unique<Asset<Animation>>(std::move(animation));
	}
};