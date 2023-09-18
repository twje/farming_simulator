#pragma once

#include <memory>

#include "Core/TextureManager.h"

class ResourceLocator
{
public:
	// Setters	
	void SetTextureManager(std::unique_ptr<TextureManager> textureManager) { mTextureManager = std::move(textureManager); }

	// Getters	
	TextureManager& GetTextureManager() const { return *mTextureManager; }

private:	
	std::unique_ptr<TextureManager> mTextureManager;
};