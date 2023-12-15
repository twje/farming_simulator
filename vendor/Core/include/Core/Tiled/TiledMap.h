#pragma once

#include "Core/Tiled/TiledMapAsset.h"

// --------------------------------------------------------------------------------
class TileLayer
{
public:
    TileLayer(const TileLayerData& layer)
        : mLayer(layer)
    { }

    const TileData& GetTile(uint32_t x, uint32_t y) const { return mLayer.GetTile(x, y); }

    const uint32_t GetDepth() { return mLayer.GetDepth(); }
    bool IsVisible() const { return mLayer.GetVisible(); }

private:
    const TileLayerData& mLayer;
};

// --------------------------------------------------------------------------------
class ObjectLayer
{
public:
    ObjectLayer(const ObjectLayerData& layer)
        : mLayer(layer)
    { }

    const std::vector<ObjectData>& GetObjects() const { return mLayer.GetObjects(); }

    const uint32_t GetDepth() { return mLayer.GetDepth(); }
    bool IsVisible() const { return mLayer.GetVisible(); }

private:
    const ObjectLayerData& mLayer;
};

// --------------------------------------------------------------------------------
class TiledMap
{
public:
    TiledMap(TiledMapAsset& map)
        : mMap(map)
    {
        for (const TileLayerData& layer : mMap.GetTileLayers())
        {            
            mTiledLayers.emplace_back(TileLayer(layer));
        }
        for (const ObjectLayerData& layer : mMap.GetObjectLayers())
        {            
            mObjectLayers.emplace_back(ObjectLayer(layer));
        }
    }

    // Getters
    uint32_t GetTileWidth() const { return mMap.GetTileWidth(); }
    uint32_t GetTileHeight() const { return mMap.GetTileHeight(); }
    uint32_t GetMapWidth() const { return mMap.GetMapWidth(); }
    uint32_t GetMapHeight() const { return mMap.GetMapHeight(); }

    std::vector<TileLayer>& GetTiledLayers() { return mTiledLayers; }
    std::vector<ObjectLayer>& GetObjectLayers() { return mObjectLayers; }

    const TextureRegion& GetTextureRegion(uint32_t globalTileId) const 
    { 
        return mMap.GetTextureRegion(globalTileId); 
    }

private:
    TiledMapAsset& mMap;
    std::vector<TileLayer> mTiledLayers;
    std::vector<ObjectLayer> mObjectLayers;
};