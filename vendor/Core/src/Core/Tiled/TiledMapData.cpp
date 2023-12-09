#include "Core/Tiled/TiledMapData.h"
#include "Core/Tiled/TiledMapAsset.h"

// --------------------------------------------------------------------------------
std::unique_ptr<TileTextureLookup> SpritesheetTiledSet::CreateTileTextureLookup() const
{
    return std::make_unique<SpritesheetData>(*this);
}

// --------------------------------------------------------------------------------
std::unique_ptr<TileTextureLookup> ImageCollectionTiledSet::CreateTileTextureLookup() const
{
    return std::make_unique<TextureListData>(*this);
}