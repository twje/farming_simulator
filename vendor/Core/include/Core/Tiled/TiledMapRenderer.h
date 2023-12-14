#pragma once

#include "Core/Tiled/TiledMap.h"
#include <algorithm>

//------------------------------------------------------------------------------
class TiledMapViewRegion
{
public:
	TiledMapViewRegion(const TiledMap& map, const sf::FloatRect& screenViewRegion)
	{
		float tileWidth = map.GetTileWidth();
		float tileHeight = map.GetTileHeight();
		float mapWidth = map.GetMapWidth();
		float mapHeight = map.GetMapHeight();
		
		mLeft = screenViewRegion.left;
		mTop = screenViewRegion.top;
		mRight = screenViewRegion.left + screenViewRegion.width;
		mBottom = screenViewRegion.top + screenViewRegion.height;

		mStartX = Clamp(0, mapWidth, mLeft) / tileWidth;
		mStartY = Clamp(0, mapHeight, mTop) / tileHeight;
		mEndX = std::ceil(static_cast<float>(Clamp(0, mapWidth, mRight)) / tileWidth);
		mEndY = std::ceil(static_cast<float>(Clamp(0, mapHeight, mBottom)) / tileHeight);
	}

	int32_t GetLeft() const { return mLeft; }
	int32_t GetTop() const { return mTop; }
	int32_t GetRight() const { return mRight; }
	int32_t GetBottom() const { return mBottom; }

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
	float mLeft;
	float mTop;
	float mRight;
	float mBottom;
	size_t mStartX;
	size_t mStartY;
	size_t mEndX;
	size_t mEndY;
};

// --------------------------------------------------------------------------------
class RenderableLayer
{
public:
    virtual void Draw(sf::RenderWindow& window, const TiledMap& map, const TiledMapViewRegion& viewRegion) = 0;
    virtual uint32_t GetDrawOrder() = 0;
	virtual bool IsVisible() = 0;
};

// --------------------------------------------------------------------------------
class RenderableTiledLayer : public RenderableLayer
{
public:
    RenderableTiledLayer(EditableTiledLayer& layer)
        : mLayer(layer)
    { }

    virtual void Draw(sf::RenderWindow& window, const TiledMap& map, const TiledMapViewRegion& viewRegion)
    {
		for (size_t y = viewRegion.GetStartY(); y < viewRegion.GetEndY(); y++)
		{
			for (size_t x = viewRegion.GetStartX(); x < viewRegion.GetEndX(); x++)
			{
				Tile tile = mLayer.GetTile(x, y);
				DrawTile(window, map, tile, x, y);
			}
		}
    }

	virtual uint32_t GetDrawOrder() { return mLayer.GetDepth(); }
	virtual bool IsVisible() { return mLayer.IsVisible(); }

private:
	void DrawTile(sf::RenderWindow& window, const TiledMap& map, const Tile& tile, size_t x, size_t y)
	{
		uint32_t globalTileId = tile.GetGlobalId();
		if (globalTileId == 0)
		{
			return;
		}

		const TextureRegion& textureRegion = map.GetTextureRegion(globalTileId);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * map.GetTileWidth(), y * map.GetTileHeight()));
		window.draw(sprite);
	}

private:
    EditableTiledLayer& mLayer;
};

// --------------------------------------------------------------------------------
class RenderableObjectLayer : public RenderableLayer
{
public:
    RenderableObjectLayer(EditableObjectLayer& layer)
        : mLayer(layer)	  
    { 
		mObjects.reserve(mLayer.GetObjects().size());
	}

    virtual void Draw(sf::RenderWindow& window, const TiledMap& map, const TiledMapViewRegion& viewRegion)
    {
		PopulateAndSortObjects();
		DrawObjects(window, map, viewRegion);
    }

	virtual uint32_t GetDrawOrder() { return mLayer.GetDepth(); }
	virtual bool IsVisible() { return mLayer.IsVisible(); }

private:
	void PopulateAndSortObjects() 
	{
		mObjects.clear();
		for (const Object& object : mLayer.GetObjects()) 
		{
			mObjects.emplace_back(&object);
		}

		std::sort(mObjects.begin(), mObjects.end(), [](const Object* a, const Object* b) {
			return CompareObjects(a, b);
		});
	}

	static bool CompareObjects(const Object* a, const Object* b) 
	{
		int aCompareValue = CalculateComparisonValue(a);
		int bCompareValue = CalculateComparisonValue(b);
		return aCompareValue < bCompareValue;
	}

	static int CalculateComparisonValue(const Object* object) 
	{
		int compareValue = object->GetY() + object->GetHeight() / 2.0f;
		if (object->GetType() == ObjectType::TILE)
		{
			compareValue -= object->GetHeight();
		}
		return compareValue;
	}

	void DrawObjects(sf::RenderWindow& window, const TiledMap& map, const TiledMapViewRegion& viewRegion)
	{
		for (const Object* object : mObjects)
		{
			if (IsObjectInRegion(*object, viewRegion, object->GetType() == ObjectType::TILE))
			{
				DrawObject(*object, window, map);
			}
		}
	}

	void DrawObject(const Object& object, sf::RenderWindow& window, const TiledMap& map) 
	{
		switch (object.GetType()) 
		{
			case ObjectType::RECTANGLE:
				DrawRectangle(object, window);
				break;
			case ObjectType::TILE:
				DrawTile(object, window, map);
				break;
		}
	}

	void DrawRectangle(const Object& object, sf::RenderWindow& window)
	{
		sf::RectangleShape rectangle;
		rectangle.setPosition(sf::Vector2f(object.GetX(), object.GetY()));
		rectangle.setSize(sf::Vector2f(object.GetWidth(), object.GetHeight()));
		rectangle.setOutlineThickness(-1.0f);
		rectangle.setOutlineColor(sf::Color(128, 128, 128, 255));
		rectangle.setFillColor(sf::Color(128, 128, 128, 64));

		window.draw(rectangle);
	}

	void DrawTile(const Object& object, sf::RenderWindow& window, const TiledMap& map)
	{
		float tileWidth = map.GetTileWidth();
		float tileHeight = map.GetTileHeight();
		int32_t x = object.GetX();
		int32_t y = object.GetY() - object.GetHeight();
		uint32_t gid = object.GetGid();

		const TextureRegion& textureRegion = map.GetTextureRegion(gid);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x, y));

		window.draw(sprite);
	}

	bool IsObjectInRegion(const Object& object, const TiledMapViewRegion& region, bool adjustY)
	{
		// Adjust Y-coordinate for the top edge of the object
		int objectTopY = adjustY ? object.GetY() - object.GetHeight() : object.GetY();

		// Object's pixel-based boundaries
		int objectLeft = object.GetX();
		int objectRight = object.GetX() + object.GetWidth();
		int objectTop = objectTopY;
		int objectBottom = objectTop + object.GetHeight();

		// Check if the object overlaps with the region
		bool overlapsHorizontally = (objectLeft < region.GetRight()) && (objectRight > region.GetLeft());
		bool overlapsVertically = (objectTop < region.GetBottom()) && (objectBottom > region.GetTop());

		return overlapsHorizontally && overlapsVertically;
	}

private:
    EditableObjectLayer& mLayer;
	std::vector<const Object*> mObjects;
};

// --------------------------------------------------------------------------------
class TiledMapRenderer
{
public:
    TiledMapRenderer(TiledMapAsset& map)
        : mMap(map)
    { 
        for (EditableTiledLayer& layer : mMap.GetTiledLayers())
        {
            mLayer.emplace_back(std::make_unique<RenderableTiledLayer>(layer));
        }
        
        for (EditableObjectLayer& layer : mMap.GetObjectLayers())
        {
            mLayer.emplace_back(std::make_unique<RenderableObjectLayer>(layer));
        }

        std::sort(mLayer.begin(), mLayer.end(), [](const auto& a, const auto& b) {
            return a->GetDrawOrder() < b->GetDrawOrder();
        });
	}

    void Draw(sf::RenderWindow& window, sf::FloatRect screenViewRegion)
    {
        TiledMapViewRegion viewRegion(mMap, screenViewRegion);
        for (auto& layerPtr : mLayer)
        {
			if (layerPtr->IsVisible())
			{
				layerPtr->Draw(window, mMap, viewRegion);
			}
        }
    }

private:
	TiledMap mMap;
    std::vector<std::unique_ptr<RenderableLayer>> mLayer;
};