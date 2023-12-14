#pragma once

#include "Core/Tiled/TiledMapAsset.h"

// --------------------------------------------------------------------------------
class EditableTiledLayer
{
public:
    EditableTiledLayer(const TiledLayer& layer)
        : mLayer(layer)
    { }

    const Tile& GetTile(uint32_t x, uint32_t y) const { return mLayer.GetTile(x, y); }

    const uint32_t GetDepth() { return mLayer.GetDepth(); }
    bool IsVisible() const { return mLayer.GetVisible(); }

private:
    const TiledLayer& mLayer;
};

// --------------------------------------------------------------------------------
class EditableObjectLayer
{
public:
    EditableObjectLayer(const ObjectLayer& layer)
        : mLayer(layer)
    { }

    const std::vector<Object>& GetObjects() const { return mLayer.GetObjects(); }

    const uint32_t GetDepth() { return mLayer.GetDepth(); }
    bool IsVisible() const { return mLayer.GetVisible(); }

private:
    const ObjectLayer& mLayer;
};

// --------------------------------------------------------------------------------
class TiledMap
{
public:
    TiledMap(TiledMapAsset& map)
        : mMap(map)
    {
        for (const TiledLayer& layer : mMap.GetTileLayers())
        {            
            mTiledLayers.emplace_back(EditableTiledLayer(layer));
        }
        for (const ObjectLayer& layer : mMap.GetObjectLayers())
        {            
            mObjectLayers.emplace_back(EditableObjectLayer(layer));
        }
    }

    // Getters
    uint32_t GetTileWidth() const { return mMap.GetTileWidth(); }
    uint32_t GetTileHeight() const { return mMap.GetTileHeight(); }
    uint32_t GetMapWidth() const { return mMap.GetMapWidth(); }
    uint32_t GetMapHeight() const { return mMap.GetMapHeight(); }

    std::vector<EditableTiledLayer>& GetTiledLayers() { return mTiledLayers; }
    std::vector<EditableObjectLayer>& GetObjectLayers() { return mObjectLayers; }

    const TextureRegion& GetTextureRegion(uint32_t globalTileId) const 
    { 
        return mMap.GetTextureRegion(globalTileId); 
    }

private:
    TiledMapAsset& mMap;
    std::vector<EditableTiledLayer> mTiledLayers;
    std::vector<EditableObjectLayer> mObjectLayers;
};