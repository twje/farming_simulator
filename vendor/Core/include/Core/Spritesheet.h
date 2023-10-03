#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/TextureRegion.h"

class SpriteSheet
{
public:
    SpriteSheet(AssetManager& assetManager, const std::string& textureId, uint16_t rows, uint16_t cols);
    const TextureRegion& GetTextureRegion(uint16_t row, uint16_t col) const;

private:
    std::vector<TextureRegion> textureRegions;
    uint16_t rows;
    uint16_t cols;
};