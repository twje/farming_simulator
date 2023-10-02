#pragma once

#include <memory>

#include "Core/TextureManager.h"
#include "Core/AnimationManager.h"
#include "Core/ApplicationConfig.h"
#include "Core/AssetManager.h"


class ResourceLocator
{
	friend class Application;

public:
	ResourceLocator(ApplicationConfig config)
		: mConfig(config),
		  mTextureManager(*this),
		  mAnimationManager(*this)
	{ }

	const ApplicationConfig& GetApplicationConfig() const { return mConfig; }
	TextureManager& GetTextureManager() { return mTextureManager; }
	AnimationManager& GetAnimationManager() { return mAnimationManager; }
	AssetManager& GetAssetManager() { return mAssetManager; }

private:
	ApplicationConfig mConfig;
	TextureManager mTextureManager;
	AnimationManager mAnimationManager;
	AssetManager mAssetManager;
};