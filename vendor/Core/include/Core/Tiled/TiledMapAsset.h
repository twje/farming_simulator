#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/Tiled/TiledMapData.h"
#include "Core/AssetManager.h"
#include "Core/Utils.h"
#include "Core/Spritesheet.h"
#include "Core/Texture.h"
#include "Core/TextureRegion.h"

// Third party
#include <SFML/Graphics.hpp>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

// Type Aliases
//------------------------------------------------------------------------------
using TileTextureLookupMap = std::unordered_map<uint32_t, std::unique_ptr<TileTextureLookup>>;

//------------------------------------------------------------------------------
class TileTextureLookup
{
public:
	// Dependency management
	virtual void GetDependencyDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors) = 0;
	virtual void ResolveDependencies(AssetManager& assetManager) = 0;
	
	virtual TextureRegion GetTextureRegion(uint32_t index) const = 0;

protected:
	std::string GenerateAssetId(const std::string& filePath)
	{
		std::hash<std::string> stringHasher;
		return std::to_string(stringHasher(filePath));
	}
};

//------------------------------------------------------------------------------
class SpritesheetData : public TileTextureLookup
{
public:
	SpritesheetData(const SpritesheetTiledSet& tileset)
		: mTileset(tileset)
		, mSpritesheet{ "", nullptr}
	{ }
	
	virtual void GetDependencyDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors) override
	{		
		AddTextureDescriptor(outDescriptors);
		AddSpritesheetDescriptor(outDescriptors);
	}

	virtual void ResolveDependencies(AssetManager& assetManager) override
	{
		mSpritesheet.second = &assetManager.GetAsset<Spritesheet>(mSpritesheet.first);
	}

	virtual TextureRegion GetTextureRegion(uint32_t index) const override
	{
		return mSpritesheet.second->GetTextureRegion(index);
	}	

private:
	void AddTextureDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors)
	{
		const std::string texFilePath = mTileset.GetImageFilePath().string();
		std::string texAssetId = GenerateAssetId(texFilePath);

		YAML::Emitter texEmitter;
		texEmitter << YAML::BeginMap << YAML::Key << "filePath" << YAML::Value << texFilePath << YAML::EndMap;
		auto texNode = YAML::Load(texEmitter.c_str());
		auto texDescriptor = std::make_unique<AssetMemoryDescriptor<Texture>>(texAssetId, texNode);
		descriptors.emplace_back(std::move(texDescriptor));
	}

	void AddSpritesheetDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors)
	{
		const std::string texFilePath = mTileset.GetImageFilePath().string();
		mSpritesheet.first = GenerateAssetId(texFilePath + "_spt");

		YAML::Emitter sptEmitter;
		sptEmitter << YAML::BeginMap;
		sptEmitter << YAML::Key << "textureId" << YAML::Value << GenerateAssetId(texFilePath);
		sptEmitter << YAML::Key << "cols" << YAML::Value << mTileset.GetColumns();
		sptEmitter << YAML::Key << "rows" << YAML::Value << mTileset.GetRows();
		sptEmitter << YAML::Key << "margin" << YAML::Value << mTileset.GetMargin();
		sptEmitter << YAML::Key << "spacing" << YAML::Value << mTileset.GetSpacing();
		sptEmitter << YAML::EndMap;

		auto sptNode = YAML::Load(sptEmitter.c_str());
		auto sptDescriptor = std::make_unique<AssetMemoryDescriptor<Spritesheet>>(mSpritesheet.first, sptNode);
		descriptors.emplace_back(std::move(sptDescriptor));
	}

private:
	const SpritesheetTiledSet& mTileset;
	std::pair<std::string, Spritesheet*> mSpritesheet;	
};

//------------------------------------------------------------------------------
class TextureListData : public TileTextureLookup
{
public:
	TextureListData(const ImageCollectionTiledSet& tileset)
		: mTileset(tileset)
	{ }

	virtual void GetDependencyDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors) override
	{
		AddTextureDescriptors(outDescriptors);
	}

	virtual void ResolveDependencies(AssetManager& assetManager) override
	{
		for (auto& pair : mTextures)
		{
			pair.second = &assetManager.GetAsset<Texture>(pair.first);
		}
	}

	virtual TextureRegion GetTextureRegion(uint32_t index) const override
	{
		Texture* texture = mTextures[index].second;
		return TextureRegion(&texture->GetRawTexture(), sf::IntRect());
	}

private:
	void AddTextureDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors)
	{		
		for (const ImageTile& imageTile : mTileset.GetImageTiles())
		{
			const std::string texFilePath = imageTile.GetImageFilePath().string();
			std::string texAssetId = GenerateAssetId(texFilePath);

			YAML::Emitter texEmitter;
			texEmitter << YAML::BeginMap << YAML::Key << "filePath" << YAML::Value << texFilePath << YAML::EndMap;
			auto texNode = YAML::Load(texEmitter.c_str());
			auto texDescriptor = std::make_unique<AssetMemoryDescriptor<Texture>>(texAssetId, texNode);
			descriptors.emplace_back(std::move(texDescriptor));

			AddTextureReference(texAssetId);
		}
	}
	
	void AddTextureReference(const std::string& assetId)
	{
		mTextures.push_back({ assetId , nullptr });
	}

private:
	const ImageCollectionTiledSet& mTileset;
	std::vector<std::pair<std::string, Texture*>> mTextures;
};

//------------------------------------------------------------------------------
class TiledMapViewRegion
{
public:
	TiledMapViewRegion(const TiledMapData& data, const sf::IntRect& screenViewRegion)
	{
		uint32_t tileWidth = data.GetTileWidth();
		uint32_t tileHeight = data.GetTileHeight();
		uint32_t mapWidth = data.GetMapWidth();
		uint32_t mapHeight = data.GetMapHeight();

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

//------------------------------------------------------------------------------
class TileTextureCollector : public TiledMapElementVisitor
{
public:
	TileTextureCollector(TileTextureLookupMap& tileTextureLookupMap)
		: mTileTextureLookupMap(tileTextureLookupMap)
	{ }

	virtual void Accept(const SpritesheetTiledSet& element) override
	{
		auto tileTextureLookup = std::make_unique<SpritesheetData>(element);
		mTileTextureLookupMap.emplace(element.GetFirstGid(), std::move(tileTextureLookup));
	}
	
	virtual void Accept(const ImageCollectionTiledSet& element) override
	{
		auto tileTextureLookup = std::make_unique<TextureListData>(element);
		mTileTextureLookupMap.emplace(element.GetFirstGid(), std::move(tileTextureLookup));
	}

private:
	TileTextureLookupMap& mTileTextureLookupMap;
};

//------------------------------------------------------------------------------
class TiledMapAsset : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMapAsset(std::unique_ptr<TiledMapData> data)
		: mData(std::move(data))
	{ }

	void Draw(sf::RenderWindow& window, const sf::IntRect& screenViewRegion)
	{
		TiledMapViewRegion viewRegion(*mData.get(), screenViewRegion);
		
		for (const TiledLayer& layer : mData->GetTiledLayers())
		{
			DrawLayer(window, layer, viewRegion);
		}
	}

	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors() override
	{
		TileTextureCollector tileTextureCollector(mTileTextureLookupMap);
		for (const TiledSet& tileset : mData->GetTiledSets())
		{
			tileset.Visit(tileTextureCollector);
		}

		std::vector<std::unique_ptr<BaseAssetDescriptor>> descriptors;
		for (const auto& pair : mTileTextureLookupMap)
		{
			pair.second->GetDependencyDescriptors(descriptors);
		}

		return descriptors;
	}

	virtual void ResolveAssetDeps(AssetManager& assetManager) override
	{
		for (auto& pair : mTileTextureLookupMap)
		{
			TileTextureLookup* tileTextureLookup = pair.second.get();
			tileTextureLookup->ResolveDependencies(assetManager);
		}
	}

	TextureRegion GetTextureRegion(uint32_t globalTileId) const
	{
		const TiledSet& tiledSet = mData->GetTiledSet(globalTileId);
		uint32_t localTileId = globalTileId - tiledSet.GetFirstGid();

		const TileTextureLookup* tileTextureLookup = mTileTextureLookupMap.at(tiledSet.GetFirstGid()).get();
		return tileTextureLookup->GetTextureRegion(localTileId);
	}

private:
	void DrawLayer(sf::RenderWindow& window, const TiledLayer& layer, const TiledMapViewRegion& viewRegion)
	{
		uint32_t tileWidth = mData->GetTileWidth();
		uint32_t tileHeight = mData->GetTileHeight();

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

		const TextureRegion& textureRegion = GetTextureRegion(globalTileId);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		window.draw(sprite);
	}

private:
	std::unique_ptr<TiledMapData> mData;
	TileTextureLookupMap mTileTextureLookupMap;
};