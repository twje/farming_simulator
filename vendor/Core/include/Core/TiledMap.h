#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>
#include <cassert>

// --------------------------------------------------------------------------------
class TiledLayer
{
public:
    TiledLayer(std::string_view name, uint32_t width, uint32_t height, std::vector<uint32_t>&& tiles)
        : mName(name),
          mWidth(width),
          mHeight(height),
          mTiles(std::move(tiles))
    {}

    const std::string_view GetName() const { return mName; }

private:
    std::string mName;
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint32_t> mTiles;
};

// --------------------------------------------------------------------------------
class TiledSet
{
public:
    TiledSet(uint32_t firstGid, uint32_t columns, uint32_t imageHeight,
             uint32_t imageWidth, uint32_t tileWidth, uint32_t tileHeight,
             uint32_t margin, uint32_t spacing, uint32_t tileCount,
             const std::string& name)
        : mFirstGid(firstGid), mColumns(columns), mImageWidth(imageWidth),
          mImageHeight(imageHeight), mTileWidth(tileWidth), mTileHeight(tileHeight),
          mMargin(margin), mSpacing(spacing), mTileCount(tileCount),
          mName(name)
    { }

    uint32_t GetFirstGid() const { return mFirstGid; }
    uint32_t GetColumns() const { return mColumns; }
    uint32_t GetImageWidth() const { return mImageWidth; }
    uint32_t GetImageHeight() const { return mImageHeight; }
    uint32_t GetTileHeight() const { return mTileWidth; }
    uint32_t GetTileWidth() const { return mTileHeight; }
    uint32_t GetMargin() const { return mMargin; }
    uint32_t GetSpacing() const { return mSpacing; }
    uint32_t GetTileCount() const { return mTileCount; }
    const std::string& GetName() const { return mName; }

private:
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
class TiledMap
{
public:
    TiledMap(uint32_t width, uint32_t height, uint32_t tileWidth, uint32_t tileHeight)
        : mWidth(width),
          mHeight(height),
          mTileWidth(tileWidth),
          mTileHeight(tileHeight)
    { }

    // Getters
    const TiledLayer& GetLayer(const std::string& name) const
    {
        auto it = mLayerLookup.find(name);
        assert(it != mLayerLookup.end() && "Layer not found");
        return GetLayer(it->second);
    }

    const TiledLayer& GetLayer(size_t layerIndex) const
    {
        assert(layerIndex < mLayers.size() && "Invalid layer index");
        return mLayers[layerIndex];
    }

    const TiledSet& GetTileSet(const std::string& name)
    {
        auto it = mTileSets.find(name);
        assert(it != mTileSets.end() && "TileSet not found");
        return it->second;
    }

    size_t GetLayerCount() const { return mLayers.size(); }

    // Setters
    TiledLayer& AddLayer(TiledLayer&& layer)
    {
        auto result = mLayerLookup.emplace(layer.GetName(), mLayers.size());
        assert(result.second && "Duplicate layer name");
        mLayers.emplace_back(std::move(layer));
        return mLayers.back();
    }

    TiledSet& AddTileset(TiledSet&& tileSet)
    {
        auto result = mTileSets.emplace(tileSet.GetName(), std::move(tileSet));
        assert(result.second && "Duplicate tileset name");
        return tileSet;
    }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mTileWidth;
    uint32_t mTileHeight;
    std::unordered_map<std::string, TiledSet> mTileSets;
    std::vector<TiledLayer> mLayers;
    std::unordered_map<std::string, size_t> mLayerLookup;
};