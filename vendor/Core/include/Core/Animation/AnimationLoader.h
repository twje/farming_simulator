#pragma once

#include "Core/AssetManager.h"

#include <memory>

class AnimationLoader : public AssetLoader
{
public:
	virtual std::unique_ptr<Asset> Load(const std::string& filePath) override;
};