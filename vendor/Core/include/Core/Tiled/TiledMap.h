#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/AssetManager.h"
#include "Core/GameObject.h"
#include "Core/Group.h"

// Third party
#include <SFML/Graphics.hpp>
#include <tileson.hpp>

// System
#include <unordered_map>
#include <vector>

// Type alias
//------------------------------------------------------------------------------
using TextureLookup = std::unordered_map<std::string, std::unique_ptr<sf::Texture>>;

//------------------------------------------------------------------------------
sf::IntRect ConvertTsonRectToSFMLIntRect(const tson::Rect& rect)
{
	return { sf::Vector2i(rect.x, rect.y), sf::Vector2i(rect.width, rect.height) };
}

//------------------------------------------------------------------------------
template<typename VECTOR_TYPE>
sf::Vector2f ConvertTsonVectorToSFMLVector2f(const VECTOR_TYPE& vector)
{
	return { static_cast<float>(vector.x), static_cast<float>(vector.y) };
}

//------------------------------------------------------------------------------
class ViewRegion
{
public:
	ViewRegion(const tson::Map& data, const sf::FloatRect& screenViewRegion)
		: mScreenViewRegion(screenViewRegion)
	{
		float tileWidth = data.getTileSize().x;
		float tileHeight = data.getTileSize().x;
		float mapWidth = data.getSize().x * tileWidth;
		float mapheight = data.getSize().y * tileHeight;

		float mLeft = screenViewRegion.left;
		float mTop = screenViewRegion.top;
		float mRight = screenViewRegion.left + screenViewRegion.width;
		float mBottom = screenViewRegion.top + screenViewRegion.height;

		mStartX = Clamp(0, mapWidth, mLeft) / tileWidth;
		mStartY = Clamp(0, mapheight, mTop) / tileHeight;
		mEndX = std::ceil(static_cast<float>(Clamp(0, mapWidth, mRight)) / tileWidth);
		mEndY = std::ceil(static_cast<float>(Clamp(0, mapheight, mBottom)) / tileHeight);
	}

	size_t GetStartX() const { return mStartX; }
	size_t GetStartY() const { return mStartY; }
	size_t GetEndX() const { return mEndX; }
	size_t GetEndY() const { return mEndY; }

	const sf::FloatRect& GetScreenViewRegion() const { return mScreenViewRegion; }

private:
	int32_t Clamp(int32_t minValue, int32_t maxValue, int32_t value)
	{
		return std::min(maxValue, std::max(minValue, value));
	}

	size_t mStartX;
	size_t mStartY;
	size_t mEndX;
	size_t mEndY;
	sf::FloatRect mScreenViewRegion;
};

//------------------------------------------------------------------------------
class TSonObjectWrapper
{
public:
	virtual ~TSonObjectWrapper() = default;
	virtual void Draw(sf::RenderWindow& window) = 0;
	virtual sf::FloatRect GetGlobalBounds() const = 0;
};

//------------------------------------------------------------------------------
template <typename T>
class ObjectPool
{
public:
	std::unique_ptr<T> acquire()
	{
		if (pool.empty())
		{
			return std::make_unique<T>();
		}

		auto obj = std::move(pool.back());
		pool.pop_back();
		return obj;
	}

	void release(std::unique_ptr<T> obj)
	{
		pool.push_back(std::move(obj));
	}

private:
	std::vector<std::unique_ptr<T>> pool;
};

//------------------------------------------------------------------------------
class TSonObject : public TSonObjectWrapper
{
public:
	TSonObject(tson::Map& map, float gid, tson::Vector2i position, TextureLookup& textureLookup)
	{
		tson::Tileset* tileset = map.getTilesetByGid(gid);
		assert(tileset->getType() == tson::TilesetType::ImageCollectionTileset);

		uint32_t id = gid - tileset->getFirstgid() + 1;
		tson::Tile* tile = tileset->getTile(id);
		const sf::Texture* texture = textureLookup.at(tile->getImage().generic_string()).get();

		mSprite = std::make_unique<sf::Sprite>(*texture);
		mSprite->setOrigin({ 0.0f, static_cast<float>(texture->getSize().y) });
		mSprite->setPosition(ConvertTsonVectorToSFMLVector2f(position));	
	}

	virtual void Draw(sf::RenderWindow& window) override
	{
		window.draw(*mSprite);
	}

	virtual sf::FloatRect GetGlobalBounds() const override
	{
		return mSprite->getGlobalBounds();
	}

private:
	std::unique_ptr<sf::Sprite> mSprite;
};

//------------------------------------------------------------------------------
class TSonObjectRectangle : public TSonObjectWrapper
{
public:
	TSonObjectRectangle(tson::Vector2i position, tson::Vector2i size)		
	{
		sf::Color solidGray(128, 128, 128, 255);
		sf::Color transparentGray(128, 128, 128, 64);		
		
		mRectangle.setSize(ConvertTsonVectorToSFMLVector2f(size));
		mRectangle.setPosition(ConvertTsonVectorToSFMLVector2f(position));
		mRectangle.setOutlineThickness(-1);
		mRectangle.setOutlineColor(solidGray);
		mRectangle.setFillColor(transparentGray);
	}
	 
	virtual void Draw(sf::RenderWindow& window) override
	{
		window.draw(mRectangle);
	}

	virtual sf::FloatRect GetGlobalBounds() const override
	{
		return mRectangle.getGlobalBounds();
	}

private:
	sf::RectangleShape mRectangle;
};

//------------------------------------------------------------------------------
class TSonObjectPoint : public TSonObjectWrapper
{
public:
	TSonObjectPoint(tson::Vector2i position)
	{
		sf::Color solidGray(128, 128, 128, 255);
		sf::Color transparentGray(128, 128, 128, 64);

		float size = 20.0f;
		sf::Vector2f sfmlPosition = ConvertTsonVectorToSFMLVector2f(position);
		
		mTriangle.setPointCount(3);
		mTriangle.setPoint(0, sf::Vector2f(0, size) + sfmlPosition);
		mTriangle.setPoint(1, sf::Vector2f(size, -size) + sfmlPosition);
		mTriangle.setPoint(2, sf::Vector2f(-size, -size) + sfmlPosition);

		mTriangle.setOutlineThickness(-1);
		mTriangle.setOutlineColor(solidGray);
		mTriangle.setFillColor(transparentGray);
	}

	virtual void Draw(sf::RenderWindow& window) override
	{
		window.draw(mTriangle);
	}

	virtual sf::FloatRect GetGlobalBounds() const override
	{
		return mTriangle.getGlobalBounds();
	}

private:
	sf::ConvexShape mTriangle;
};

//------------------------------------------------------------------------------
class SpriteAdpater : public TSonObjectWrapper
{
public:
	void Assign(Sprite* sprite)
	{
		mSprite = sprite;
	}

	virtual void Draw(sf::RenderWindow& window) override
	{
		window.draw(*mSprite);
	}

	virtual sf::FloatRect GetGlobalBounds() const override
	{
		return mSprite->GetGlobalBounds();
	}

private:
	Sprite* mSprite = nullptr;
};

//------------------------------------------------------------------------------
class SpriteAdapterManager
{
public:
	SpriteAdapterManager() = default;

	SpriteAdpater* AcquireSpriteAdapter(Sprite* sprite) 
	{
		auto spriteAdapterPtr = mPool.acquire();
		spriteAdapterPtr->Assign(sprite);
		
		mCache.push_back(std::move(spriteAdapterPtr));
		return mCache.back().get();
	}

	void ReleaseAllSpriteAdapters() 
	{
		for (auto& spriteAdapterPtr : mCache)
		{
			mPool.release(std::move(spriteAdapterPtr));
		}
		mCache.clear();
	}

private:
	ObjectPool<SpriteAdpater> mPool;
	std::vector<std::unique_ptr<SpriteAdpater>> mCache;
};

//------------------------------------------------------------------------------
class TiledMapObjectDefinition
{
public:
	TiledMapObjectDefinition(tson::Object& data, sf::Texture& texture)
		: mData(data)
		, mTexture(texture)
	{ }

	tson::Object& GetData() const { return mData; }
	sf::Texture& GetTexture() const { return mTexture; }

private:
	tson::Object& mData;
	sf::Texture& mTexture;
};

//------------------------------------------------------------------------------
class TiledMap : public Asset
{
public:
	static std::unique_ptr<TiledMap> LoadFromDefinitionFile(const fs::path& defintionFilepath)
	{
		YAML::Node node = YAML::LoadFile(defintionFilepath.generic_string());
		fs::path mapFilepath = defintionFilepath.parent_path() / node["filepath"].as<std::string>();
		
		// Load tiled map data
		tson::Tileson parser;
		std::unique_ptr<tson::Map> data = parser.parse(mapFilepath);
		if (data->getStatus() != tson::ParseStatus::OK)
		{
			std::cerr << "Failed to load tiled map: " + data->getStatusMessage() << std::endl;
			return nullptr;			
		}		

		// Populate objects in a layer to be rendered
		std::unordered_map<uint32_t, bool> renderObjectsInLayer;
		for (tson::Layer& layer : data->getLayers())
		{
			uint32_t layerId = static_cast<uint32_t>(layer.getId());
			renderObjectsInLayer[layerId] = true;
		
			for (const auto& layerNameNode : node["noRenderObjectsInLayer"])
			{
				tson::Layer* noRenderLayer = data->getLayer(layerNameNode.as<std::string>());
				if (noRenderLayer && noRenderLayer->getId() == layer.getId()) 
				{
					renderObjectsInLayer[layerId] = false;
					break;
				}
			}
		}

		// Initialize tiled map
		auto result = std::make_unique<TiledMap>(std::move(data));
		
		for (tson::Layer& layer : result->mData->getLayers())
		{			
			uint32_t layerId = static_cast<uint32_t>(layer.getId());
			if (renderObjectsInLayer[layerId])
			{
				result->PopulateObjectsInLayer(layer);
			}			
		}
		
		return result;
	}

	explicit TiledMap(std::unique_ptr<tson::Map> data)
		: mData(std::move(data))
		, mText(mFont)
	{
		mFont.loadFromFile("../../graphics/fonts/Inter/Inter-VariableFont_slnt,wght.ttf");  // temporary

		for (tson::Tileset& tileset : mData->getTilesets())
		{
			ProcessTileset(tileset);
		}

		for (tson::Layer& layer : mData->getLayers())
		{
			ProcessLayer(layer);
		}		
	}

	void GetObjectDefinitionsInLayer(const std::string& layerName, std::vector<std::unique_ptr<TiledMapObjectDefinition>>& outDefinitions)
	{	
		tson::Layer* layer = mData->getLayer(layerName);
		if (layer)
		{
			for (tson::Object& object : layer->getObjects())
			{
				if (object.getObjectType() != tson::ObjectType::Object)
				{
					continue;
				}

				tson::Tileset* tileset = mData->getTilesetByGid(object.getGid());
				assert(tileset->getType() == tson::TilesetType::ImageCollectionTileset);

				uint32_t id = object.getGid() - tileset->getFirstgid() + 1;
				tson::Tile* tile = tileset->getTile(id);
				sf::Texture* texture = mTextures.at(tile->getImage().generic_string()).get();

				outDefinitions.push_back(std::make_unique<TiledMapObjectDefinition>(object, *texture));
			}
		}
	}

	void Update(const sf::Time& timestamp)
	{
		for (const auto& pair : mAnimationUpdateQueue)
		{
			// Time needs to be received as microseconds to get the right precision		
			float ms = (float)((double)timestamp.asMicroseconds() / 1000);		
			pair.second->update(ms);
		}
	}

	void Draw(sf::RenderWindow& window, const sf::FloatRect& screenViewRegion, Group& group)
	{		
		ViewRegion viewRegion(*mData.get(), screenViewRegion);
		for (size_t index = 0; index < LayerCount(); index++)
		{
			DrawLayerAtIndex(window, viewRegion, index, group);
		}
	}

	void DrawLayerAtIndex(sf::RenderWindow& window, const ViewRegion& viewRegion, size_t index, Group& group)
	{
		tson::Layer& layer = mData->getLayers().at(index);		
		if (layer.isVisible())
		{
			DrawLayer(window, viewRegion, layer, group);
		}
	}

	size_t LayerCount() { return mData->getLayers().size(); }

private:
	void DrawLayer(sf::RenderWindow& window, const ViewRegion& viewRegion, tson::Layer& layer, Group& group)
	{
		switch (layer.getType())
		{
			case tson::LayerType::TileLayer:
			{
				DrawTileLayer(window, viewRegion, layer);
				break;
			}
			case tson::LayerType::ObjectGroup:
			{
				DrawObjectLayer(window, viewRegion, layer, group);
				break;
			}			
			default:
			{
				std::cerr << "Layer not supported" << std::endl;
				break;
			}
		}
	}

	void DrawTileLayer(sf::RenderWindow& window, const ViewRegion& viewRegion, tson::Layer& layer)
	{
		auto& tileObjects = layer.getTileObjects();

		const tson::Vector2i& mapSize = layer.getMap()->getSize();
		for (size_t yIndex = viewRegion.GetStartY(); yIndex < viewRegion.GetEndY(); yIndex++)
		{
			for (size_t xIndex = viewRegion.GetStartX(); xIndex < viewRegion.GetEndX(); xIndex++)
			{
				std::tuple<int, int> index(xIndex, yIndex);
				if (tileObjects.find(index) == tileObjects.end())
				{
					continue;
				}

				tson::TileObject& tileObject = tileObjects.at(index);
			
				tson::Tile* tile = tileObject.getTile();
				assert(tile->getFlipFlags() == tson::TileFlipFlags::None);

				tson::Tileset* tileset = tile->getTileset();
				assert(tileset->getType() == tson::TilesetType::ImageTileset);

				// Animation
				sf::IntRect textureRegion = ConvertTsonRectToSFMLIntRect(tileObject.getDrawingRect());
				if (mAnimationUpdateQueue.find(tile->getGid()) != mAnimationUpdateQueue.end())
				{
					assert(tileObject.getTile()->getAnimation().any());
					uint32_t animationTileId = tileObject.getTile()->getAnimation().getCurrentTileId();
					tson::Tile* animationTile = tileset->getTile(animationTileId);
					textureRegion = ConvertTsonRectToSFMLIntRect(animationTile->getDrawingRect());
				}

				std::string textureFilepath = tileset->getFullImagePath().generic_string();
				const sf::Texture* texture = mTextures.at(textureFilepath).get();
				sf::Sprite sprite(*texture, textureRegion);

				sprite.setPosition(ConvertTsonVectorToSFMLVector2f(tileObject.getPosition()));

				window.draw(sprite);						
			}
		}				
	}

	void DrawObjectLayer(sf::RenderWindow& window, const ViewRegion& viewRegion, tson::Layer& layer, Group& group)
	{
		// Add external objects to sort list
		mObjectSortList.clear();
		for (GameObject* gameObject : group)
		{
			/*if (gameObject->IsMarkedForRemoval())  // TODO: fix bug 
			{ 
				continue; 
			}*/

			TSonObjectWrapper* spriteAdapterPtr = mSpriteAdapterManager.AcquireSpriteAdapter(static_cast<Sprite*>(gameObject));
			mObjectSortList.push_back(spriteAdapterPtr);
		}
		
		// Populate tiled map objects
		for (tson::Object& object : layer.getObjects())
		{			
			assert(object.getFlipFlags() == tson::TileFlipFlags::None);		
			
			auto pairPtr = mObjects.find(object.getId());
			if (pairPtr != mObjects.end())
			{
				TSonObjectWrapper* objectWrapper = pairPtr->second.get();
				if (objectWrapper->GetGlobalBounds().findIntersection(viewRegion.GetScreenViewRegion()))
				{
					mObjectSortList.push_back(objectWrapper);
				}
			}
		}

		// Sort
		std::sort(mObjectSortList.begin(), mObjectSortList.end(), [](const TSonObjectWrapper* a, const TSonObjectWrapper* b)
		{
			return a->GetGlobalBounds().getCenter().y < b->GetGlobalBounds().getCenter().y;
		});
		
		// Draw
		for (TSonObjectWrapper* object : mObjectSortList)
		{
			object->Draw(window);
		}

		// Release external objects from pool
		mSpriteAdapterManager.ReleaseAllSpriteAdapters();
	}

	void DrawText(sf::RenderWindow& window, const std::string& text, sf::Vector2f position, float width)
	{
		mText.setString(text);
		mText.setCharacterSize(32);
		mText.setScale(sf::Vector2f(0.5f, 0.5f));
		mText.setFillColor(sf::Color::Black);
		mText.setStyle(sf::Text::Bold);

		sf::FloatRect textBounds = mText.getGlobalBounds();
		position.x += (width - textBounds.width) / 2.0f;
		position.y -= textBounds.height + 10;
		mText.setPosition(position);

		window.draw(mText);
	}

	void ProcessTileset(tson::Tileset& tileset) 
	{
		if (tileset.getType() == tson::TilesetType::ImageTileset) 
		{			
			CacheTexture(tileset.getFullImagePath().generic_string());						
		}
		else if (tileset.getType() == tson::TilesetType::ImageCollectionTileset) 
		{
			for (const auto& tile : tileset.getTiles()) 
			{				
				CacheTexture(tile.getImage().generic_string());
			}
		}
	}

	void ProcessLayer(tson::Layer& layer)
	{
		// Animations
		for (auto& pair : layer.getTileObjects())
		{
			tson::Tile* tile = pair.second.getTile();
			if (tile->getAnimation().any())
			{
				assert(tile->getTileset()->getType() == tson::TilesetType::ImageTileset);
				assert(tile->getId() != 0);
				mAnimationUpdateQueue[tile->getGid()] = &tile->getAnimation();
			}
		}
	}

	void PopulateObjectsInLayer(tson::Layer& layer)
	{
		if (layer.getType() == tson::LayerType::ObjectGroup)
		{
			for (tson::Object& object : layer.getObjects())
			{
				switch (object.getObjectType())
				{
					case tson::ObjectType::Object:
					{
						mObjects[object.getId()] = std::make_unique<TSonObject>(*mData.get(),
																				object.getGid(),
																			    object.getPosition(),
																			    mTextures);
						break;
					}
					case tson::ObjectType::Rectangle:
					{
						mObjects[object.getId()] = std::make_unique<TSonObjectRectangle>(object.getPosition(),
																						 object.getSize());
						break;
					}
					case tson::ObjectType::Point:
					{
						mObjects[object.getId()] = std::make_unique<TSonObjectPoint>(object.getPosition());
						break;
					}
				}
			}
		}
	}

	void CacheTexture(const std::string& filepath)
	{
		auto texture = std::make_unique<sf::Texture>();

		if (!texture->loadFromFile(filepath))
		{
			throw std::runtime_error("TiledMap: Failed to load texture from path: " + filepath);
		}

		if (!mTextures.emplace(filepath, std::move(texture)).second)
		{
			std::cerr << "TiledMap: Texture already loaded from path: " + filepath << std::endl;
		}
	}
	
	std::unique_ptr<tson::Map> mData;
	TextureLookup mTextures;	
	std::unordered_map<uint32_t, tson::Animation*> mAnimationUpdateQueue;	
	std::vector<TSonObjectWrapper*> mObjectSortList;	
	std::unordered_map<uint32_t, std::unique_ptr<TSonObjectWrapper>> mObjects;
	SpriteAdapterManager mSpriteAdapterManager;
	sf::Font mFont;
	sf::Text mText;
};

//------------------------------------------------------------------------------
class TiledMapLoader : public AssetLoader<TiledMap>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<TiledMap> descriptor) override
	{
		auto result = TiledMap::LoadFromDefinitionFile(descriptor.GetFilePath());
		assert(result);
		return result;
	}
};