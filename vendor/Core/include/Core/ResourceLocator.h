#pragma once

#include <memory>

#include "Core/TextureManager.h"
#include "Core/AnimationManager.h"
#include "Core/ApplicationConfig.h"

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

private:
	ApplicationConfig mConfig;
	TextureManager mTextureManager;
	AnimationManager mAnimationManager;
};