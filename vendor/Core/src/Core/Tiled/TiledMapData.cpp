#include "Core/Tiled/TiledMapData.h"

// --------------------------------------------------------------------------------
/*virtual*/ void SpritesheetTiledSet::Visit(TiledMapElementVisitor& visitor) const
{
    visitor.Accept(*this);
}

// --------------------------------------------------------------------------------
/*virtual*/ void ImageCollectionTiledSet::Visit(TiledMapElementVisitor& visitor) const
{
    visitor.Accept(*this);
}

// --------------------------------------------------------------------------------
/*virtual*/ void TiledLayer::Visit(TiledMapElementVisitor& visitor) const
{
    visitor.Accept(*this);
}