#pragma once

// Includes
//------------------------------------------------------------------------------
#include <string>
#include <string_view>
#include <cassert>
#include <filesystem>
#include <vector>

// Namespaces
//------------------------------------------------------------------------------
namespace fs = std::filesystem;

//------------------------------------------------------------------------------
class LayerAttributes
{
    friend class LayerData;

public:
    LayerAttributes(std::uint32_t id, std::string_view name, bool visible, uint32_t depth)
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

//------------------------------------------------------------------------------
class LayerData
{
public:
    LayerData(LayerAttributes&& layerAttributes)
        : mLayerAttributes(std::move(layerAttributes))
    { }

    uint32_t GetId() const { return mLayerAttributes.mId; }
    const std::string_view GetName() const { return mLayerAttributes.mName; }
    bool GetVisible() const { return mLayerAttributes.mVisible; }
    uint32_t GetDepth() const { return mLayerAttributes.mDepth; }

private:
    LayerAttributes mLayerAttributes;
};

//------------------------------------------------------------------------------
class TileData
{
public:
    TileData(uint32_t gid)
        : mGid(gid)
    { }

    uint32_t GetGlobalId() const { return mGid; }

private:
    uint32_t mGid;
};

//------------------------------------------------------------------------------
class TileLayerData : public LayerData
{
public:
    TileLayerData(LayerData&& layerData, uint32_t width, uint32_t height, std::vector<TileData>&& tiles)
        : LayerData(std::move(layerData)),
        mWidth(width),
        mHeight(height),
        mTiles(std::move(tiles))
    { }

    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    const TileData& GetTile(uint32_t x, uint32_t y) const { return mTiles.at(x + y * GetWidth()); }

private:
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<TileData> mTiles;
};

//------------------------------------------------------------------------------
enum class ObjectType : uint32_t
{
    RECTANGLE = 0,
    POINT = 1,
    POLYGON = 2,
    ELLIPSE = 3,
    TILE = 4,
};

//------------------------------------------------------------------------------
class ObjectAttributes
{
    friend class ObjectData;

public:
    ObjectAttributes(const std::string& name, uint32_t id, uint32_t width, uint32_t height, uint32_t rotation,
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

//------------------------------------------------------------------------------
class ObjectData
{
public:
    ObjectData(ObjectAttributes&& data, ObjectType type, uint32_t gid = 0)
        : mObjectAttributes(std::move(data))
        , mType(type)
        , mGid(gid)
    { }

    const std::string& GetName() const { return mObjectAttributes.mName; }
    uint32_t GetId() const { return mObjectAttributes.mId; }
    uint32_t GetWidth() const { return mObjectAttributes.mWidth; }
    uint32_t GetHeight() const { return mObjectAttributes.mHeight; }
    uint32_t GetRotation() const { return mObjectAttributes.mRotation; }
    bool GetVisible() const { return mObjectAttributes.mVisible; }
    int32_t GetX() const { return mObjectAttributes.mX; }
    int32_t GetY() const { return mObjectAttributes.mY; }
    ObjectType GetType() const { return mType; }
    uint32_t GetGid() const { return mGid; }

private:
    ObjectAttributes mObjectAttributes;
    ObjectType mType;
    uint32_t mGid;
};

//------------------------------------------------------------------------------
class ObjectLayerData : public LayerData
{
public:
    ObjectLayerData(LayerData&& layerData, std::vector<ObjectData>&& objects)
        : LayerData(std::move(layerData))
        , mObjects(std::move(objects))
    { }

    const std::vector<ObjectData>& GetObjects() const { return mObjects; }

private:
    std::vector<ObjectData> mObjects;
};

//------------------------------------------------------------------------------
class TilesetAttributes
{
    friend class TilesetData;

public:
    TilesetAttributes(const std::string& name, uint32_t firstGid, uint32_t tileWidth, uint32_t tileHeight,
        uint32_t columns, uint32_t margin, uint32_t spacing, uint32_t tileCount)
        : mName(name)
        , mFirstGid(firstGid)
        , mTileWidth(tileWidth)
        , mTileHeight(tileHeight)
        , mColumns(columns)
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

// TilesetData
//------------------------------------------------------------------------------
class TilesetData
{
public:
    TilesetData(TilesetAttributes&& tilesetAttributes)
        : mTilesetAttributes(std::move(tilesetAttributes))
    { }

    const std::string& GetName() const { return mTilesetAttributes.mName; }
    uint32_t GetFirstGid() const { return mTilesetAttributes.mFirstGid; }
    uint32_t GetTileWidth() const { return mTilesetAttributes.mTileWidth; }
    uint32_t GetTileHeight() const { return mTilesetAttributes.mTileHeight; }
    uint32_t GetColumns() const { return mTilesetAttributes.mColumns; }
    uint32_t GetMargin() const { return mTilesetAttributes.mMargin; }
    uint32_t GetSpacing() const { return mTilesetAttributes.mSpacing; }
    uint32_t GetTileCount() const { return mTilesetAttributes.mTileCount; }

private:
    TilesetAttributes mTilesetAttributes;
};

//------------------------------------------------------------------------------
class SpritesheetTilesetData : public TilesetData
{
public:
    SpritesheetTilesetData(TilesetData&& tilesetData, const fs::path& imageFilePath, uint32_t imageHeight, uint32_t imageWidth)
        : TilesetData(std::move(tilesetData)),
        mImageFilePath(imageFilePath),
        mImageWidth(imageWidth),
        mImageHeight(imageHeight)
    { }

    const fs::path& GetImageFilePath() const { return mImageFilePath; }
    uint32_t GetImageWidth() const { return mImageWidth; }
    uint32_t GetImageHeight() const { return mImageHeight; }
    uint32_t GetRows() const { return GetTileCount() / GetColumns(); }

private:
    fs::path mImageFilePath;
    uint32_t mImageWidth;
    uint32_t mImageHeight;
};

//------------------------------------------------------------------------------
class TilesetImageData
{
public:
    TilesetImageData(uint32_t id, const fs::path& imageFilePath, uint32_t imageHeight, uint32_t imageWidth)
        : mId(id)
        , mImageFilePath(imageFilePath)
        , mImageHeight(imageHeight)
        , mImageWidth(imageWidth)
    { }

    const uint32_t GetId() const { return mId; }
    const fs::path& GetImageFilePath() const { return mImageFilePath; }

private:
    uint32_t mId;
    fs::path mImageFilePath;
    uint32_t mImageHeight;
    uint32_t mImageWidth;
};

//------------------------------------------------------------------------------
class ImageCollectionTilesetData : public TilesetData
{
public:
    ImageCollectionTilesetData(TilesetData&& tilesetData, std::vector<TilesetImageData>&& imageTiles)
        : TilesetData(std::move(tilesetData))
        , mImageTiles(std::move(imageTiles))
    { }

    const std::vector<TilesetImageData>& GetImageTiles() const { return mImageTiles; }

private:
    std::vector<TilesetImageData> mImageTiles;
};

// TiledMapData
//------------------------------------------------------------------------------
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

    void AddSpritesheetTileset(SpritesheetTilesetData&& tileset)
    {
        mSpritesheetTilesets.emplace_back(std::move(tileset));
    }

    void AddImageCollectionTileset(ImageCollectionTilesetData&& tileset)
    {
        mImageCollectionTilesets.emplace_back(std::move(tileset));
    }

    void AddTileLayer(TileLayerData&& layer)
    {
        mTileLayers.emplace_back(std::move(layer));
    }

    void AddObjectLayer(ObjectLayerData&& layer)
    {
        mObjectLayers.emplace_back(std::move(layer));
    }

    const std::vector<TileLayerData>& GetTileLayers() const
    {
        return mTileLayers;
    }

    const std::vector<ObjectLayerData>& GetObjectLayers() const
    {
        return mObjectLayers;
    }

    const std::vector<SpritesheetTilesetData>& GetSpritesheetTilesets() const
    {
        return mSpritesheetTilesets;
    }

    const std::vector<ImageCollectionTilesetData>& GetImageCollectionTilesets() const
    {
        return mImageCollectionTilesets;
    }

    const fs::path& GetFilePath() const { return mFilePath; }
    uint32_t GetTileWidth() const { return mTileWidth; }
    uint32_t GetTileHeight() const { return mTileHeight; }
    uint32_t GetMapWidth() const { return mWidth * mTileWidth; }
    uint32_t GetMapHeight() const { return mHeight * mTileHeight; }

private:
    fs::path mFilePath;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mTileWidth;
    uint32_t mTileHeight;
    std::vector<TileLayerData> mTileLayers;
    std::vector<ObjectLayerData> mObjectLayers;
    std::vector<SpritesheetTilesetData> mSpritesheetTilesets;
    std::vector<ImageCollectionTilesetData> mImageCollectionTilesets;
};