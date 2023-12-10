#pragma once

#include "Core/Tiled/TiledMapAsset.h"

// --------------------------------------------------------------------------------
class TiledMapRenderer
{
public:
    TiledMapRenderer(TiledMapAsset& map)
        : mMap(map)
    { }

    void Draw(sf::RenderWindow& window, const sf::IntRect& screenViewRegion)
    {
        mMap.Draw(window, screenViewRegion);
    }

private:
    TiledMapAsset& mMap;
};