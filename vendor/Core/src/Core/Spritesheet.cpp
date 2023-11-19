#include "Core/Spritesheet.h"

// Includes
//------------------------------------------------------------------------------
// Project
#include "Core/Texture.h"
#include "Core/TextureRegion.h"

// ----------------------------------------------------------
Spritesheet::Spritesheet(const std::string& textureId, uint16_t rows, uint16_t cols)
    : mTextureId(textureId),
      mRows(rows),
      mCols(cols)
{ }

// ----------------------------------------------------------
/*virtual*/ void Spritesheet::ResolveAssetDeps(AssetManager& assetManager)
{
    ComputeTextureRegions(assetManager);    
}

// ----------------------------------------------------------
const TextureRegion& Spritesheet::GetTextureRegion(uint16_t row, uint16_t col) const
{
    uint16_t index = row * mCols + col;
    return textureRegions[index];
}

// ----------------------------------------------------------
const TextureRegion& Spritesheet::GetTextureRegion(uint16_t index) const
{
    return textureRegions[index];
}

// ----------------------------------------------------------
void Spritesheet::ComputeTextureRegions(AssetManager& assetManager)
{
    sf::Texture& texture = assetManager.GetAsset<Texture>(mTextureId).GetRawTexture();
    sf::Vector2i tileSize(texture.getSize().x / mCols, texture.getSize().y / mRows);
    for (uint16_t row = 0; row < mRows; ++row)
    {
        for (uint16_t col = 0; col < mCols; ++col)
        {
            sf::Vector2i position(col * tileSize.x, row * tileSize.y);            
            TextureRegion textureRegion(&texture, sf::IntRect(position, tileSize));
            textureRegions.emplace_back(textureRegion);
        }
    }
}

// ----------------------------------------------------------
void Spritesheet::SaveToFile(const std::string& filePath)
{
    YAML::Emitter emitter;
    Serialize(emitter);
    std::ofstream file(filePath);
    file << emitter.c_str();
}

// ----------------------------------------------------------
std::unique_ptr<Spritesheet> Spritesheet::LoadFromFile(const std::string& filePath)
{
    YAML::Node node = YAML::LoadFile(filePath);
    return Deserialize(node);
}

// ----------------------------------------------------------
void Spritesheet::Serialize(YAML::Emitter& emitter)
{
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "textureId" << YAML::Value << mTextureId;
    emitter << YAML::Key << "rows" << YAML::Value << mRows;
    emitter << YAML::Key << "cols" << YAML::Value << mCols;
    emitter << YAML::EndMap;
}

// ----------------------------------------------------------
std::unique_ptr<Spritesheet> Spritesheet::Deserialize(const YAML::Node& node)
{
    const std::string& textureId = node["textureId"].as<std::string>();
    uint16_t rows = node["rows"].as<uint16_t>();
    uint16_t cols = node["cols"].as<uint16_t>();
    
    return std::make_unique<Spritesheet>(textureId, rows, cols);
}