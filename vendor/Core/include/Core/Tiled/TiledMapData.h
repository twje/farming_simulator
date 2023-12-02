#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <map>
#include <memory>
#include <cassert>
#include <optional>
#include <functional>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

// --------------------------------------------------------------------------------
class TiledLayer
{
public:
    TiledLayer(std::uint32_t id, std::string_view name, uint32_t width, uint32_t height, std::vector<uint32_t>&& tiles)
        : mId(id),
          mName(name),
          mWidth(width),
          mHeight(height),
          mTiles(std::move(tiles))
    {}

    uint32_t GetId() const { return mId; }
    const std::string_view GetName() const { return mName; }
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    uint32_t GetTile(uint32_t x, uint32_t y) const { return mTiles.at(x + y * mWidth); }

private:
    uint32_t mId;
    std::string mName;
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint32_t> mTiles;
};

// --------------------------------------------------------------------------------
class TiledSet
{
public:
    TiledSet(const fs::path& imageFilePath, uint32_t firstGid, uint32_t columns, uint32_t imageHeight,
             uint32_t imageWidth, uint32_t tileWidth, uint32_t tileHeight,
             uint32_t margin, uint32_t spacing, uint32_t tileCount,
             const std::string& name)
        : mImageFilePath(imageFilePath), mFirstGid(firstGid), mColumns(columns), mImageWidth(imageWidth),
          mImageHeight(imageHeight), mTileWidth(tileWidth), mTileHeight(tileHeight),
          mMargin(margin), mSpacing(spacing), mTileCount(tileCount),
          mName(name)
    { }

    const fs::path& GetImageFilePath() const { return mImageFilePath; }
    uint32_t GetFirstGid() const { return mFirstGid; }
    uint32_t GetColumns() const { return mColumns; }
    uint32_t GetRows() const { return mTileCount / mColumns; }
    uint32_t GetImageWidth() const { return mImageWidth; }
    uint32_t GetImageHeight() const { return mImageHeight; }
    uint32_t GetTileHeight() const { return mTileWidth; }
    uint32_t GetTileWidth() const { return mTileHeight; }
    uint32_t GetMargin() const { return mMargin; }
    uint32_t GetSpacing() const { return mSpacing; }
    uint32_t GetTileCount() const { return mTileCount; }
    const std::string& GetName() const { return mName; }

private:
    fs::path mImageFilePath;
    uint32_t mFirstGid;
    uint32_t mColumns;
    uint32_t mImageWidth;
    uint32_t mImageHeight;
    uint32_t mTileWidth;
    uint32_t mTileHeight;
    uint32_t mMargin;
    uint32_t mSpacing;
    uint32_t mTileCount;
    std::string mName;
};

// --------------------------------------------------------------------------------
class TiledMapData
{
public:
    TiledMapData(const fs::path& filePath, uint32_t width, uint32_t height, uint32_t tileWidth, uint32_t tileHeight)
        : mFilePath(filePath),
          mWidth(width),
          mHeight(height),
          mTileWidth(tileWidth),
          mTileHeight(tileHeight)
    { }

    // Setters
    TiledLayer& AddLayer(TiledLayer&& layer)
    {
        assert(!IsLayerAdded(layer.GetId()) && "Duplicate Layer");
        mLayers.emplace_back(std::move(layer));
        return mLayers.back();
    }

    TiledSet& AddTileset(TiledSet&& tileSet)
    {
        auto result = mTileSets.emplace(tileSet.GetFirstGid(), std::move(tileSet));
        assert(result.second && "Duplicate TileSet");
        return result.first->second;
    }

    // Getters
    fs::path GetFilePath() { return mFilePath; }
    uint32_t GetTileWidth() const { return mWidth; }
    uint32_t GetTileHeight() const { return mHeight; }

    std::vector<std::reference_wrapper<const TiledSet>> GetTiledSets() const
    {
        std::vector<std::reference_wrapper<const TiledSet>> tileSets;
        for (const auto& pair : mTileSets)
        {
            tileSets.push_back(pair.second);
        }
        return tileSets;
    }

    const std::vector<TiledLayer>& GetTiledLayers() const
    {
        return mLayers;
    }

    const TiledSet& GetTileSet(uint32_t globalTileId) const
    {     
        assert(globalTileId != 0 && "Invalid tile id");
        const TiledSet* result = nullptr;

        uint32_t closestValue = std::numeric_limits<uint32_t>::max();
        for (const auto& pair : mTileSets)
        {            
            if (globalTileId >= pair.first)
            {   
                uint32_t distance = globalTileId - pair.first;
                if (distance <= closestValue)
                {
                    closestValue = distance;
                    result = &pair.second;
                }                
            }
        }
        return *result;
    }

private:
    bool IsLayerAdded(uint32_t layerId)
    {
        auto itr = std::find_if(mLayers.begin(), mLayers.end(), [layerId](const TiledLayer& obj)
        {
            return obj.GetId() == layerId;
        });
        return itr != mLayers.end();
    }

private:
    fs::path mFilePath;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mTileWidth;
    uint32_t mTileHeight;
    std::vector<TiledLayer> mLayers;
    std::map<uint32_t, TiledSet> mTileSets;
};