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
class TiledMapElementVisitor;

// Namespaces
// --------------------------------------------------------------------------------
namespace fs = std::filesystem;

// --------------------------------------------------------------------------------
// LAYERS
// --------------------------------------------------------------------------------
class LayerData
{
    friend class Layer;

public:
    LayerData(std::uint32_t id, std::string_view name, bool visible, uint32_t depth)
        : mId(id),
          mName(name),
          mVisible(visible),
          mDepth(depth)
    { }

private:
    uint32_t mId;
    std::string mName;
    bool mVisible;
    uint32_t mDepth;
};

// --------------------------------------------------------------------------------
class Layer
{
public:
    Layer(LayerData&& layerData)
        : mLayerData(std::move(layerData))
    { }

    uint32_t GetId() const { return mLayerData.mId; }
    const std::string_view GetName() const { return mLayerData.mName; }
    bool GetVisible() const { return mLayerData.mVisible; }
    uint32_t GetDepth() const { return mLayerData.mDepth; }

private:
    LayerData mLayerData;
};

// --------------------------------------------------------------------------------
class Tile
{
public:
    Tile(uint32_t gid)
        : mGid(gid)
    { }

    uint32_t GetGlobalId() const { return mGid; }    

private:
    uint32_t mGid;
};

// --------------------------------------------------------------------------------
class TiledLayer : public Layer
{
public:
    TiledLayer(LayerData&& layerData, uint32_t width, uint32_t height, std::vector<Tile>&& tiles)
        : Layer(std::move(layerData)),
          mWidth(width),
          mHeight(height),
          mTiles(std::move(tiles))
    { }
    
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    const Tile& GetTile(uint32_t x, uint32_t y) const { return mTiles.at(x + y * GetWidth()); }    

private:
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<Tile> mTiles;
};

// --------------------------------------------------------------------------------
enum class ObjectType : uint32_t
{
    RECTANGLE = 0,
    POINT = 1,
    POLYGON = 2,
    ELLIPSE = 3,
    TILE = 4,
};

// --------------------------------------------------------------------------------
class ObjectData
{
    friend class Object;

public:
    ObjectData(const std::string& name, uint32_t id, uint32_t width, uint32_t height, uint32_t rotation,
               bool visible, int32_t x, int32_t y)
        : mName(name)
        , mId(id)        
        , mWidth(width)
        , mHeight(height)
        , mRotation(rotation)
        , mVisible(visible)
        , mX(x)
        , mY(y)
    { }

private:
    std::string mName;
    uint32_t mId;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mRotation;
    bool mVisible;
    int32_t mX;
    int32_t mY;
};

// --------------------------------------------------------------------------------
class Object
{
public:
    Object(ObjectData&& data, ObjectType type, uint32_t gid=0)
        : mData(data)
        , mType(type)
        , mGid(gid)
    { }

    const std::string& GetName() const { return mData.mName; }
    uint32_t GetId() const { return mData.mId; }
    uint32_t GetWidth() const { return mData.mWidth; }
    uint32_t GetHeight() const { return mData.mHeight; }
    uint32_t GetRotation() const { return mData.mRotation; }
    bool GetVisible() const { return mData.mVisible; }
    int32_t GetX() const { return mData.mX; }
    int32_t GetY() const { return mData.mY; }    
    ObjectType GetType() const { return mType; }

    // temp
    uint32_t GetGid() const { return mGid; }

private:
    ObjectData mData;
    ObjectType mType;
    
    // Temp
    uint32_t mGid;
};

// --------------------------------------------------------------------------------
class ObjectLayer : public Layer
{
public:
    ObjectLayer(LayerData&& layerData, std::vector<Object>&& objects)
        : Layer(std::move(layerData))
        , mObjects(objects)
    { }

    const std::vector<Object>& GetObjects() const { return mObjects; }

private:
    std::vector<Object> mObjects;
};

// --------------------------------------------------------------------------------
// TILESETS
// --------------------------------------------------------------------------------
class TiledSetData
{
    friend class TiledSet;

public:
    TiledSetData(const std::string& name, uint32_t firstGid, uint32_t tileWidth, uint32_t tileHeight, 
                 uint32_t columns, uint32_t margin, uint32_t spacing, uint32_t tileCount)
        : mName(name)
        , mFirstGid(firstGid)
        , mTileWidth(tileWidth)
        , mTileHeight(tileHeight), 
          mColumns(columns)
        , mMargin(margin)
        , mSpacing(spacing)
        , mTileCount(tileCount)
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
class TiledSet
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

private:
    fs::path mImageFilePath;    
    uint32_t mImageWidth;
    uint32_t mImageHeight;
};

// --------------------------------------------------------------------------------
class ImageTile  // TODO: confusing as this belongs to a tiledset not a layer (rename)
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
    void AddSpritesheetTiledSet(SpritesheetTiledSet&& tileset)
    {
        mSpritesheetTilesets.emplace_back(std::move(tileset));
    }

    void AddImageCollectionTiledSet(ImageCollectionTiledSet&& tileset)
    {
        mImageCollectionTilesets.emplace_back(std::move(tileset));
    }  

    void AddTileLayer(TiledLayer&& layer)
    {
        mTileLayers.emplace_back(std::move(layer));
    }

    void AddObjectLayer(ObjectLayer&& layer)
    {
        mObjectLayers.emplace_back(std::move(layer));
    }

    // Getters
    const std::vector<TiledLayer>& GetTilLayers() const
    {
        return mTileLayers;
    }

    const std::vector<ObjectLayer>& GetObjectLayers() const
    {
        return mObjectLayers;
    }

    const std::vector<SpritesheetTiledSet>& GetSpritesheetTilesets() const
    {
        return mSpritesheetTilesets;
    }

    const std::vector<ImageCollectionTiledSet>& GetImageCollectionTilesets() const
    {
        return mImageCollectionTilesets;
    }

    fs::path GetFilePath() { return mFilePath; }
    uint32_t GetTileWidth() const { return mTileWidth; }
    uint32_t GetTileHeight() const { return mTileHeight; }
    uint32_t GetMapWidth() const { return mWidth * mTileWidth; }
    uint32_t GetMapHeight() const { return mHeight * mTileHeight; }

private:
    // Map data
    fs::path mFilePath;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mTileWidth;
    uint32_t mTileHeight;

    // Layers
    std::vector<TiledLayer> mTileLayers;
    std::vector<ObjectLayer> mObjectLayers;

    // Tilesets
    std::vector<SpritesheetTiledSet> mSpritesheetTilesets;
    std::vector<ImageCollectionTiledSet> mImageCollectionTilesets;
};