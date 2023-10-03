#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/TextureRegion.h"

class SpriteSheet
{
public:
    SpriteSheet(AssetManager& assetManager, const std::string& textureId, uint16_t rows, uint16_t cols)
        : rows(rows),
          cols(cols)
    {
        sf::Texture& texture = assetManager.GetAsset<sf::Texture>(textureId);
        sf::Vector2u tileSize(texture.getSize().x / cols, texture.getSize().y / rows);
        for (uint16_t row = 0; row < rows; ++row)
        {
            for (uint16_t col = 0; col < cols; ++col)
            {
                sf::Vector2i position(col * tileSize.x, row * tileSize.y);
                sf::Vector2i size(texture.getSize());
                TextureRegion textureRegion(&texture, sf::IntRect(position, size));
                textureRegions.emplace_back(textureRegion);
            }
        }
    }

    const TextureRegion& GetTextureRegion(uint16_t row, uint16_t col) const
    {
        uint16_t index = row * cols + col;
        return textureRegions[index];
    }

private:
    std::vector<TextureRegion> textureRegions;
    uint16_t rows;
    uint16_t cols;
};