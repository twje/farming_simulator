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

//------------------------------------------------------------------------------
sf::IntRect ConvertTsonRectToSFMLIntRect(const tson::Rect& rect);

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
	ViewRegion(const sf::Vector2f& tileSize, const sf::Vector2f& mapSize, const sf::FloatRect& screenViewRegion)
		: mScreenViewRegion(screenViewRegion)
	{
		float left = screenViewRegion.left;
		float top = screenViewRegion.top;
		float right = screenViewRegion.left + screenViewRegion.width;
		float bottom = screenViewRegion.top + screenViewRegion.height;

		mStartX = Clamp(0, mapSize.x, left) / tileSize.x;
		mStartY = Clamp(0, mapSize.y, top) / tileSize.y;
		mEndX = std::ceil(static_cast<float>(Clamp(0, mapSize.x, right)) / tileSize.x);
		mEndY = std::ceil(static_cast<float>(Clamp(0, mapSize.y, bottom)) / tileSize.y);
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
enum class LayerType : uint8_t
{
	Undefined = 0,
	TileLayer = 1,
	ObjectGroup = 2,
	ImageLayer = 3,
	Group = 4
};

//------------------------------------------------------------------------------
class TiledMapObjectDefinition
{
public:
	TiledMapObjectDefinition(const std::string& name, sf::Texture* texture, const sf::IntRect textureRegion, const sf::Vector2f& size, const sf::Vector2f& origin, const sf::Vector2f& position)
		: mName(name)
		, mTexture(texture)
		, mTextureRegion(textureRegion)
		, mSize(size)
		, mOrigin(origin)
		, mPosition(position)
	{ }
	
	const std::string& GetName() const { return mName; }
	const sf::Vector2f& GetPosition() const { return mPosition; }
	const sf::IntRect& GetTextureRegion() const { return mTextureRegion; }
	const sf::Vector2f GetSize() const { return mSize; }
	const sf::Texture* GetTexture() const { return mTexture; }
	const sf::Vector2f& GetOrigin() const { return mOrigin; }

private:
	std::string mName;
	sf::Texture* mTexture;
	sf::IntRect mTextureRegion;
	sf::Vector2f mSize;
	sf::Vector2f mPosition;	
	sf::Vector2f mOrigin;
};

//------------------------------------------------------------------------------
class TiledMapTextureManager
{
public:
	void LoadTextures(tson::Map& map) 
	{
		for (tson::Tileset& tileset : map.getTilesets()) 
		{
			LoadTilesetTextures(tileset);
		}
	}

	sf::Texture& GetTexture(uint32_t gid) 
	{
		return *mTextureLookup.at(gid);
	}

private:
	void LoadTilesetTextures(tson::Tileset& tileset)
	{
		if (tileset.getType() == tson::TilesetType::ImageTileset) 
		{
			sf::Texture* texture = LoadTextureFromFile(tileset.getFullImagePath());
			for (const auto& tile : tileset.getTiles())
			{
				mTextureLookup[tile.getGid()] = texture;
			}
		}
		else if (tileset.getType() == tson::TilesetType::ImageCollectionTileset) 
		{
			for (const auto& tile : tileset.getTiles()) 
			{				
				sf::Texture* texture = LoadTextureFromFile(tile.getImage());
				mTextureLookup[tile.getGid()] = texture;
			}
		}
	}

	sf::Texture* LoadTextureFromFile(const fs::path& filepath)
	{
		auto texture = std::make_unique<sf::Texture>();
		if (!texture->loadFromFile(filepath)) 
		{
			throw std::runtime_error("Failed to load texture from path: " + filepath.generic_string());
		}

		sf::Texture* tileTexture = texture.get();
		mTextures.emplace(filepath.generic_string(), std::move(texture));
		return tileTexture;
	}

	std::unordered_map<std::string, std::unique_ptr<sf::Texture>> mTextures;
	std::unordered_map<uint32_t, sf::Texture*> mTextureLookup;
};

//------------------------------------------------------------------------------
class TiledMap : public Asset
{
public:
	explicit TiledMap(std::unique_ptr<tson::Map> data)
		: mData(std::move(data))
	{
		mTextureManager.LoadTextures(*mData);

		// Animations
		for (tson::Layer& layer : mData->getLayers())
		{
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
	}	

	std::vector<TiledMapObjectDefinition> GetObjectDefinitions(std::string layerName)
	{
		std::vector<TiledMapObjectDefinition> definitions;			

		if (tson::Layer* layer = mData->getLayer(layerName))
		{
			// Iterate objects
			if (layer->getType() == tson::LayerType::ObjectGroup)
			{
				for (tson::Object& object : layer->getObjects())
				{				
					sf::Texture* texture = nullptr;
					sf::IntRect textureRegion;
					sf::Vector2f size;
					sf::Vector2f origin;


					if (object.getObjectType() == tson::ObjectType::Object)
					{
						tson::Tile* tile = GetTileByGid(object.getGid());

						texture = &mTextureManager.GetTexture(object.getGid());
						textureRegion = ConvertTsonRectToSFMLIntRect(tile->getDrawingRect());
						size = ConvertTsonVectorToSFMLVector2f(textureRegion.getSize());
						origin.y = 1;
					}
					if (object.getObjectType() == tson::ObjectType::Rectangle)
					{
						size = ConvertTsonVectorToSFMLVector2f(object.getSize());
					}

					definitions.emplace_back(object.getName(),
											 texture,
											 textureRegion,
											 size,
											 origin,
											 ConvertTsonVectorToSFMLVector2f(object.getPosition()));
				}
			}
			// Iterate tiles
			else if (layer->getType() == tson::LayerType::TileLayer)
			{
				for (auto& pair : layer->getTileData())
				{
					tson::Tile* tile = pair.second;
					sf::Texture* texture = &mTextureManager.GetTexture(tile->getGid());
					sf::Vector2f origin(0, 0);

					sf::IntRect textureRegion = ConvertTsonRectToSFMLIntRect(tile->getDrawingRect());
					definitions.emplace_back("",
											 texture,
											 textureRegion,
											 ConvertTsonVectorToSFMLVector2f(textureRegion.getSize()),
											 sf::Vector2f(0, 0),
											 ConvertTsonVectorToSFMLVector2f(tile->getPosition(pair.first)));

				}
			}
		}
		
		return definitions;
	}

	size_t LayerCount() { return mData->getLayers().size(); }

	LayerType GetLayerType(size_t layerIndex) 
	{ 
		tson::Layer& layer = mData->getLayers().at(layerIndex);
		return static_cast<LayerType>(layer.getType());
	}

	std::optional<size_t> GetLayerIndex(const std::string& layerName)
	{
		const std::vector<tson::Layer>& layers = mData->getLayers();
		for (size_t index = 0; index < layers.size(); ++index)
		{
			if (layers[index].getName() == layerName)
			{
				return index;
			}
		}
		return std::nullopt; // Or return InvalidIndex if not using C++17.
	}

	sf::Vector2f GetTileSize()
	{
		const tson::Vector2i& tileSize = mData->getTileSize();
		return { static_cast<float>(tileSize.x), static_cast<float>(tileSize.y) };
	}

	sf::Vector2f GetMapSize()
	{
		const tson::Vector2i& tileSize = mData->getTileSize();
		const tson::Vector2i& mapSize = mData->getSize();
		
		float xComponent = static_cast<float>(mapSize.x * tileSize.x);
		float yComponent = static_cast<float>(mapSize.y * tileSize.y);

		return { xComponent, yComponent };
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

	void DrawLayer(size_t layerIndex, sf::RenderWindow& window, const ViewRegion& viewRegion)
	{		
		tson::Layer& layer = mData->getLayers().at(layerIndex);
		if (layer.isVisible())
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
					DrawObjectLayer(window, viewRegion, layer);
					break;
				}
			}
		}
	}	

private:
	tson::Tile* GetTileByGid(uint32_t gid)
	{
		tson::Tileset* tileset = mData->getTilesetByGid(gid);
		uint32_t id = gid - tileset->getFirstgid() + 1;
		return tileset->getTile(id);
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
				
				const sf::Texture& texture = mTextureManager.GetTexture(tile->getGid());
				sf::Sprite sprite(texture, textureRegion);

				sprite.setPosition(ConvertTsonVectorToSFMLVector2f(tileObject.getPosition()));

				window.draw(sprite);
			}
		}
	}

	void DrawObjectLayer(sf::RenderWindow& window, const ViewRegion& viewRegion, tson::Layer& layer)
	{
		for (tson::Object& object : layer.getObjects())
		{
			assert(object.getFlipFlags() == tson::TileFlipFlags::None);

			sf::Vector2f position = ConvertTsonVectorToSFMLVector2f(object.getPosition());

			switch (object.getObjectType())
			{
				case tson::ObjectType::Object:
				{
					DrawObject(window, object.getGid(), position);
					break;
				}
				case tson::ObjectType::Rectangle:
				{
					DrawRectangle(window, position, 
								  ConvertTsonVectorToSFMLVector2f(object.getSize()));
					break;
				}
				case tson::ObjectType::Point:
				{
					DrawTriangle(window, position);
					break;
				}
			}
		}
	}

	void DrawObject(sf::RenderWindow& window, uint32_t gid, const sf::Vector2f& position)
	{
		tson::Tileset* tileset = mData->getTilesetByGid(gid);
		assert(tileset->getType() == tson::TilesetType::ImageCollectionTileset);

		const sf::Texture& texture = mTextureManager.GetTexture(gid);
		sf::Sprite sprite(texture);
		sprite.setOrigin({ 0.0f, static_cast<float>(texture.getSize().y) });
		sprite.setPosition(position);

		window.draw(sprite);
	}

	void DrawRectangle(sf::RenderWindow& window, const sf::Vector2f& position, const sf::Vector2f size)
	{
		sf::Color solidGray(128, 128, 128, 255);
		sf::Color transparentGray(128, 128, 128, 64);

		sf::RectangleShape rectangle;
		rectangle.setSize(ConvertTsonVectorToSFMLVector2f(size));
		rectangle.setPosition(position);
		rectangle.setOutlineThickness(-1);
		rectangle.setOutlineColor(solidGray);
		rectangle.setFillColor(transparentGray);

		window.draw(rectangle);
	}

	void DrawTriangle(sf::RenderWindow& window, const sf::Vector2f& position)
	{
		sf::Color solidGray(128, 128, 128, 255);
		sf::Color transparentGray(128, 128, 128, 64);

		float size = 20.0f;		

		sf::ConvexShape triangle;
		triangle.setPointCount(3);
		triangle.setPoint(0, sf::Vector2f(0, size) + position);
		triangle.setPoint(1, sf::Vector2f(size, -size) + position);
		triangle.setPoint(2, sf::Vector2f(-size, -size) + position);

		triangle.setOutlineThickness(-1);
		triangle.setOutlineColor(solidGray);
		triangle.setFillColor(transparentGray);

		window.draw(triangle);
	}

	std::unique_ptr<tson::Map> mData;
	TiledMapTextureManager mTextureManager;
	std::unordered_map<uint32_t, tson::Animation*> mAnimationUpdateQueue;
};

//------------------------------------------------------------------------------
class TiledMapLoader : public AssetLoader<TiledMap>
{
public:
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<TiledMap> descriptor) override
	{
		tson::Tileson parser;
		std::unique_ptr<tson::Map> data = parser.parse(descriptor.GetFilePath());
		if (data->getStatus() != tson::ParseStatus::OK)
		{
			std::cerr << "Failed to load tiled map: " + data->getStatusMessage() << std::endl;
			return nullptr;
		}
		return std::make_unique<TiledMap>(std::move(data));
	}
};