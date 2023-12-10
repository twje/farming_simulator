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
class TileLayerRenderer : public TiledMapElementVisitor
{
public:
	TileLayerRenderer(sf::RenderWindow& window, TiledMapAsset& map, const sf::IntRect& screenViewRegion)
		: mWindow(window)
		, mMap(map)
		, mViewRegion(map, screenViewRegion)
	{ }

	void Render(const Layer& layer) { layer.Visit(*this); }

private:
	virtual void Accept(const TiledLayer& tiledLayer) override
	{
		for (size_t y = mViewRegion.GetStartY(); y < mViewRegion.GetEndY(); y++)
		{
			for (size_t x = mViewRegion.GetStartX(); x < mViewRegion.GetEndX(); x++)
			{
				Tile tile = tiledLayer.GetTile(x, y);
				DrawTile(tile, x, y, mMap.GetTileWidth(), mMap.GetTileHeight());
			}
		}
	}

	void DrawTile(const Tile& tile, size_t x, size_t y, uint32_t tileWidth, uint32_t tileHeight)
	{
		uint32_t globalTileId = tile.GetGlobalId();
		if (globalTileId == 0)
		{
			return;
		}

		const TextureRegion& textureRegion = mMap.GetTextureRegion(globalTileId);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		mWindow.draw(sprite);
	}

private:
	sf::RenderWindow& mWindow;
	TiledMapAsset& mMap;
	TiledMapViewRegion mViewRegion;
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
		TileLayerRenderer renderer(window, mMap, screenViewRegion);
        for (const TiledLayer& layer : mMap.GetTiledLayers())
        {
			renderer.Render(layer);
        }
    }

private:
    TiledMapAsset& mMap;
};