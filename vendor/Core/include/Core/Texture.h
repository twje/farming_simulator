#pragma once

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"

class Texture : public Asset
{
public:    
    Texture(sf::Texture&& texture)
        : mTexture(std::move(texture))
    { }

    const sf::Texture& GetRawTexture() const { return mTexture; }
    sf::Texture& GetRawTexture() { return mTexture; }

    // Asset interface
    virtual void ResolveAssetDepsImpl(AssetManager& assetManager) { /* Nothing to do */ }

private:
    sf::Texture mTexture;
};