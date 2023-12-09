#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <algorithm>
#include <string>
#include <string_view>
#include <map>
#include <cassert>
#include <filesystem>
#include <limits>

// Forward declarations
// --------------------------------------------------------------------------------
class TileTextureLookup;
class TiledMapElementVisitor;

// Namespaces
// --------------------------------------------------------------------------------
namespace fs = std::filesystem;

// --------------------------------------------------------------------------------
class TiledMapElement
{
public:
    virtual void Visit(TiledMapElementVisitor& visitor) const = 0;
};

// --------------------------------------------------------------------------------
class LayerData
{
    friend class Layer;

public:
    LayerData(std::uint32_t id, std::string_view name)
        : mId(id),
          mName(name)          
    { }

private:
    uint32_t mId;
    std::string mName;
};

// --------------------------------------------------------------------------------
class Layer : public TiledMapElement
{
public:
    Layer(LayerData&& layerData)
        : mLayerData(std::move(layerData))
    { }

    uint32_t GetId() const { return mLayerData.mId; }
    const std::string_view GetName() const { return mLayerData.mName; }    

private:
    LayerData mLayerData;
};

// --------------------------------------------------------------------------------
class TiledLayer : public Layer
{
public:
    TiledLayer(LayerData&& layerData, uint32_t height, uint32_t width, std::vector<uint32_t>&& tiles)
        : Layer(std::move(layerData)),
          mWidth(width),
          mHeight(height),
          mTiles(std::move(tiles))
    { }
    
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    uint32_t GetTile(uint32_t x, uint32_t y) const { return mTiles.at(x + y * GetWidth()); }    

    virtual void Visit(TiledMapElementVisitor& visitor) const override;

private:
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint32_t> mTiles;
};

// --------------------------------------------------------------------------------
class TiledSetData
{
    friend class TiledSet;

public:
    TiledSetData(const std::string& name, uint32_t firstGid, uint32_t tileWidth, uint32_t tileHeight, 
                 uint32_t columns, uint32_t margin, uint32_t spacing, uint32_t tileCount)
        : mName(name), mFirstGid(firstGid), mTileWidth(tileWidth), mTileHeight(tileHeight), 
          mColumns(columns), mMargin(margin), mSpacing(spacing), mTileCount(tileCount)
    { }

private:
    std::string mName;
    uint32_t mFirstGid;
    uint32_t mTileWidth;
    uint32_t mTileHeight;
    uint32_t mColumns;
    uint32_t mMargin;
    uint32_t mSpacing;
    uint32_t mTileCount;
};

// --------------------------------------------------------------------------------
class TiledSet : public TiledMapElement
{
public:
    TiledSet(TiledSetData&& tiledSetData)
        : mTiledSetData(std::move(tiledSetData))
    { }    

    const std::string& GetName() const { return mTiledSetData.mName; }
    uint32_t GetFirstGid() const { return mTiledSetData.mFirstGid; }
    uint32_t GetTileWidth() const { return mTiledSetData.mTileWidth; }
    uint32_t GetTileHeight() const { return mTiledSetData.mTileHeight; }
    uint32_t GetColumns() const { return mTiledSetData.mColumns; }
    uint32_t GetMargin() const { return mTiledSetData.mMargin; }
    uint32_t GetSpacing() const { return mTiledSetData.mSpacing; }
    uint32_t GetTileCount() const { return mTiledSetData.mTileCount; }    
   
private:
    TiledSetData mTiledSetData;
};

// --------------------------------------------------------------------------------
class SpritesheetTiledSet : public TiledSet
{
public:    
    SpritesheetTiledSet(TiledSetData&& tiledSetData, const fs::path& imageFilePath, uint32_t imageHeight, uint32_t imageWidth)
        : TiledSet(std::move(tiledSetData)), 
          mImageFilePath(imageFilePath), 
          mImageWidth(imageWidth),
          mImageHeight(imageHeight)
    { }

    // Getters
    const fs::path& GetImageFilePath() const { return mImageFilePath; }
    uint32_t GetImageWidth() const { return mImageWidth; }
    uint32_t GetImageHeight() const { return mImageHeight; }
    uint32_t GetRows() const { return GetTileCount() / GetColumns(); }

    virtual void Visit(TiledMapElementVisitor& visitor) const override;

private:
    fs::path mImageFilePath;    
    uint32_t mImageWidth;
    uint32_t mImageHeight;
};

// --------------------------------------------------------------------------------
class ImageTile
{
public:
    ImageTile(uint32_t mId, const fs::path& imagefilePath, uint32_t imageHeight, uint32_t imageWidth)
        : mId(mId)
        , mImagefilePath(imagefilePath)
        , mImageHeight(imageHeight)
        , mImageWidth(imageWidth)
    { }

    const uint32_t GetId() const { return mId; }
    const fs::path& GetImageFilePath() const { return mImagefilePath; }

private:
    uint32_t mId;
    fs::path mImagefilePath;
    uint32_t mImageHeight;
    uint32_t mImageWidth;
};

// --------------------------------------------------------------------------------
class ImageCollectionTiledSet : public TiledSet
{
public:
    ImageCollectionTiledSet(TiledSetData&& tiledSetData, std::vector<ImageTile>&& imageTiles)
        : TiledSet(std::move(tiledSetData))
        , mImageTiles(imageTiles)
    { }

    const std::vector<ImageTile>& GetImageTiles() const { return mImageTiles; }

    virtual void Visit(TiledMapElementVisitor& visitor) const override;

private:
    std::vector<ImageTile> mImageTiles;
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

    void AddTiledSet(std::unique_ptr<TiledSet> tileSet)
    {
        auto result = mTiledSetMap.emplace(tileSet->GetFirstGid(), std::move(tileSet));
        assert(result.second && "Duplicate Spritesheet TileSet");        
    }    

    // Getters
    fs::path GetFilePath() { return mFilePath; }
    uint32_t GetTileWidth() const { return mTileWidth; }
    uint32_t GetTileHeight() const { return mTileHeight; }
    uint32_t GetMapWidth() const { return mWidth * mTileWidth; }
    uint32_t GetMapHeight() const { return mHeight * mTileHeight; }

    const std::vector<TiledLayer>& GetTiledLayers() const
    {
        return mLayers;
    }

    const TiledSet& GetTiledSet(uint32_t globalTileId) const
    {
        assert(globalTileId != 0 && "Invalid tile id");
        const TiledSet* result = nullptr;

        uint32_t closestValue = std::numeric_limits<uint32_t>::max();
        for (const auto& pair : mTiledSetMap)
        {
            if (globalTileId >= pair.first)
            {
                uint32_t distance = globalTileId - pair.first;
                if (closestValue >= distance)
                {
                    closestValue = distance;
                    result = pair.second.get();
                }
            }
        }
        return *result;
    }

    std::vector<std::reference_wrapper<const TiledSet>> GetTiledSets() const
    {
        std::vector<std::reference_wrapper<const TiledSet>> tileSets;
        for (const auto& pair : mTiledSetMap)
        {
            tileSets.push_back(*pair.second.get());
        }
        return tileSets;
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
    std::map<uint32_t, std::unique_ptr<TiledSet>> mTiledSetMap;
};

// --------------------------------------------------------------------------------
class TiledMapElementVisitor
{
public:
    virtual void Accept(const TiledLayer& element) { }
    virtual void Accept(const SpritesheetTiledSet& element) { }
    virtual void Accept(const ImageCollectionTiledSet& element) { }
};