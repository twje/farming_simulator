#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <vector>

// Third party
#include <SFML/Graphics.hpp>

// Project
#include "Core/AssetManager.h"
#include "Core/ISerializable.h"

class TextureRegion;

class Spritesheet : public Asset
{
public:    
    Spritesheet(const std::string& textureId, uint16_t rows, uint16_t cols);

    // Asset interface
    void ResolveAssetDeps(AssetManager& assetManager) override;

    const TextureRegion& GetTextureRegion(uint16_t row, uint16_t col) const;
    const TextureRegion& GetTextureRegion(uint16_t index) const;

    // IO
    void SaveToFile(const std::string& filePath);
    static std::unique_ptr<Spritesheet> LoadFromFile(const std::string& filePath);

    // ISerialize interface
    void Serialize(YAML::Emitter& emitter);
    static std::unique_ptr<Spritesheet> Deserialize(const YAML::Node& node);

private:
    void ComputeTextureRegions(AssetManager& assetManager);

private:
    std::vector<TextureRegion> textureRegions;
    std::string mTextureId;
    uint16_t mRows;
    uint16_t mCols;
};