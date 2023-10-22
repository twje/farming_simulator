#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/TextureRegion.h"
#include "Core/ISerializable.h"

class Spritesheet : public ISerializable
{
public:
    Spritesheet() = default;
    Spritesheet(AssetManager& assetManager, const std::string& textureId, uint16_t rows, uint16_t cols);

    const TextureRegion& GetTextureRegion(uint16_t row, uint16_t col) const;
    const TextureRegion& GetTextureRegion(uint16_t index) const;

    // IO
    void SaveToFile(const std::string& filePath);
    void LoadFromFile(const std::string& filePath, AssetManager& assetManager);

    // ISerialize interface
    void Serialize(YAML::Emitter& emitter) override;
    void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
    void ComputeTextureRegions(AssetManager& assetManager);

private:
    std::vector<TextureRegion> textureRegions;
    std::string mTextureId;
    uint16_t mRows;
    uint16_t mCols;
};