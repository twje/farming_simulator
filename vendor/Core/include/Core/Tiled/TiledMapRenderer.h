#pragma once

#include "Core/Tiled/TiledMapAsset.h"

//------------------------------------------------------------------------------
class TiledMapViewRegion
{
public:
	TiledMapViewRegion(const TiledMapAsset& map, const sf::IntRect& screenViewRegion)
	{
		uint32_t tileWidth = map.GetTileWidth();
		uint32_t tileHeight = map.GetTileHeight();
		uint32_t mapWidth = map.GetMapWidth();
		uint32_t mapHeight = map.GetMapHeight();

		uint32_t left = screenViewRegion.left;
		uint32_t top = screenViewRegion.top;
		uint32_t right = screenViewRegion.left + screenViewRegion.width;
		uint32_t bottom = screenViewRegion.top + screenViewRegion.height;

		mStartX = Clamp(0, mapWidth, left) / tileWidth;
		mStartY = Clamp(0, mapHeight, top) / tileHeight;
		mEndX = std::ceil(static_cast<float>(Clamp(0, mapWidth, right)) / tileWidth);
		mEndY = std::ceil(static_cast<float>(Clamp(0, mapHeight, bottom)) / tileHeight);
	}

	size_t GetStartX() const { return mStartX; }
	size_t GetStartY() const { return mStartY; }
	size_t GetEndX() const { return mEndX; }
	size_t GetEndY() const { return mEndY; }

private:
	int32_t Clamp(int32_t minValue, int32_t maxValue, int32_t value)
	{
		return std::min(maxValue, std::max(minValue, value));
	}

private:
	size_t mStartX;
	size_t mStartY;
	size_t mEndX;
	size_t mEndY;
};

// --------------------------------------------------------------------------------
class TiledMapRenderer
{
public:
    TiledMapRenderer(TiledMapAsset& map)
        : mMap(map)
    { }    

    void Draw(sf::RenderWindow& window, const sf::IntRect& screenViewRegion)
    {
        TiledMapViewRegion viewRegion(mMap, screenViewRegion);

        for (const TiledLayer& layer : mMap.GetTiledLayers())
        {
            DrawLayer(window, layer, viewRegion);
        }
    }

private:
	void DrawLayer(sf::RenderWindow& window, const TiledLayer& layer, const TiledMapViewRegion& viewRegion)
	{
		uint32_t tileWidth = mMap.GetTileWidth();
		uint32_t tileHeight = mMap.GetTileHeight();

		for (size_t y = viewRegion.GetStartY(); y < viewRegion.GetEndY(); y++)
		{
			for (size_t x = viewRegion.GetStartX(); x < viewRegion.GetEndX(); x++)
			{
				DrawTile(window, layer, x, y, tileWidth, tileHeight);
			}
		}
	}

	void DrawTile(sf::RenderWindow& window, const TiledLayer& layer, size_t x, size_t y, uint32_t tileWidth, uint32_t tileHeight)
	{
		uint32_t globalTileId = layer.GetTile(x, y).GetGlobalId();
		if (globalTileId == 0)
		{
			return;
		}

		const TextureRegion& textureRegion = mMap.GetTextureRegion(globalTileId);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		window.draw(sprite);
	}

private:
    TiledMapAsset& mMap;
};