#pragma once

#include <memory>

#include "Core/TextureManager.h"
#include "Core/AnimationManager.h"

class ResourceLocator
{
public:
	ResourceLocator()
		: mTextureManager(*this),
		  mAnimationManager(*this)
	{ }

	TextureManager& GetTextureManager() { return mTextureManager; }
	AnimationManager& GetAnimationManager() { return mAnimationManager; }

private:
	TextureManager mTextureManager;
	AnimationManager mAnimationManager;
};