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

// System
#include <filesystem>
#include <functional>
#include <iostream>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------
class TileTextureLookup
{
public:
	virtual void Resolve(AssetManager& assetManager) = 0;
	virtual TextureRegion GetTextureRegion(uint32_t index) const = 0;
};

//------------------------------------------------------------------------------
class SpritesheetData : public TileTextureLookup
{
public:	
	SpritesheetData(const std::string& assetId)
		: mAssetId(assetId)
	{ }
	
	virtual void Resolve(AssetManager& assetManager) override
	{
		mSpritesheet = &assetManager.GetAsset<Spritesheet>(mAssetId);
	}

	virtual TextureRegion GetTextureRegion(uint32_t index) const override
	{
		return mSpritesheet->GetTextureRegion(index);
	}	

private:
	std::string mAssetId;
	Spritesheet* mSpritesheet{ nullptr };
};

//------------------------------------------------------------------------------
class TextureListData : public TileTextureLookup
{
public:
	void AddTextureReference(const std::string& assetId)
	{ 
		mTextures.push_back({ assetId , nullptr });
	}

	virtual void Resolve(AssetManager& assetManager) override
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
	std::string mAssetId;
	std::vector<std::pair<std::string, Texture*>> mTextures;
};

//------------------------------------------------------------------------------
class TiledMapAsset : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMapAsset(std::unique_ptr<TiledMapData> data)
		: mData(std::move(data))
	{ }

	void Draw(sf::RenderWindow& window, const sf::IntRect& region)
	{
		for (const TiledLayer& layer : mData->GetTiledLayers())
		{
			DrawLayer(window, layer, region);
		}
	}

	// Asset interface
	virtual void ResolveAssetDeps(AssetManager& assetManager)
	{
		for (auto& pair : mTileTextureLookupMap)
		{
			TileTextureLookup* spritesheetData = pair.second.get();
			spritesheetData->Resolve(assetManager);
		}
	}

	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors()
	{
		std::vector<std::unique_ptr<BaseAssetDescriptor>> descriptors;
		for (const SpritesheetTiledSet& tileset : mData->GetSpritesheetTiledSets())
		{
			AddTextureDescriptor(descriptors, tileset);
			AddSpritesheetDescriptor(descriptors, tileset);
		}

		for (const ImageCollectionTiledSet& tileset : mData->GetImageCollectionTiledSets())
		{
			AddTextureDescriptor(descriptors, tileset);
		}

		return descriptors;
	}

	const SpritesheetTiledSet& GetSpritesheetTiledSet(uint32_t globalTileId) const
	{
		return mData->GetSpritesheetTiledSet(globalTileId);
	}

	const TextureRegion& GetTextureRegion(uint32_t firstGid, uint32_t globalTileId)
	{
		const TileTextureLookup* tileTextureLookup = mTileTextureLookupMap.at(firstGid).get();
		uint32_t localTileId = globalTileId - firstGid;
		return tileTextureLookup->GetTextureRegion(localTileId);
	}

private:
	void DrawLayer(sf::RenderWindow& window, const TiledLayer& layer, const sf::IntRect& region)
	{	
		uint32_t tileWidth = mData->GetTileWidth();
		uint32_t tileHeight = mData->GetTileHeight();
		uint32_t mapWidth = mData->GetMapWidth();
		uint32_t mapHeight = mData->GetMapHeight();

		size_t startX = Clamp(0, mapWidth, region.left) / tileWidth;
		size_t startY = Clamp(0, mapHeight, region.top) / tileHeight;
		size_t endX = std::ceil(static_cast<float>(Clamp(0, mapWidth, region.left + region.width)) / tileWidth);
		size_t endY = std::ceil(static_cast<float>(Clamp(0, mapHeight, region.top + region.height)) / tileHeight);

		for (size_t y = startY; y < endY; y++)
		{
			for (size_t x = startX; x < endX; x++)
			{
				DrawTile(window, layer, x, y, tileWidth, tileHeight);
			}
		}
	}

	void DrawTile(sf::RenderWindow& window, const TiledLayer& layer, size_t x, size_t y, uint32_t tileWidth, uint32_t tileHeight) 
	{
		uint32_t globalTileId = layer.GetTile(x, y);
		if (globalTileId == 0)
		{
			return;
		}

		const SpritesheetTiledSet& tiledSet = mData->GetSpritesheetTiledSet(globalTileId);
		const TileTextureLookup* tileTextureLookup = mTileTextureLookupMap.at(tiledSet.GetFirstGid()).get();
		uint32_t localTileId = globalTileId - tiledSet.GetFirstGid();
		
		const TextureRegion& textureRegion = tileTextureLookup->GetTextureRegion(localTileId);
		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		window.draw(sprite);
	}

	int32_t Clamp(int32_t minValue, int32_t maxValue, int32_t value)
	{
		return std::min(maxValue, std::max(minValue, value));
	}

	void AddTextureDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors, const ImageCollectionTiledSet& tileset)
	{
		TextureListData* textureListData = new TextureListData();
		for (const ImageTile& imageTile : tileset.GetImageTiles())
		{
			const std::string texFilePath = imageTile.GetImageFilePath().string();
			std::string texAssetId = GenerateAssetId(texFilePath);

			YAML::Emitter texEmitter;
			texEmitter << YAML::BeginMap << YAML::Key << "filePath" << YAML::Value << texFilePath << YAML::EndMap;
			auto texNode = YAML::Load(texEmitter.c_str());
			auto texDescriptor = std::make_unique<AssetMemoryDescriptor<Texture>>(texAssetId, texNode);
			descriptors.emplace_back(std::move(texDescriptor));

			textureListData->AddTextureReference(texAssetId);
		}
		mTileTextureLookupMap.emplace(tileset.GetFirstGid(), textureListData);
	}

	void AddTextureDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors, const SpritesheetTiledSet& tileset)
	{
		const std::string texFilePath = tileset.GetImageFilePath().string();
		std::string texAssetId = GenerateAssetId(texFilePath);

		YAML::Emitter texEmitter;
		texEmitter << YAML::BeginMap << YAML::Key << "filePath" << YAML::Value << texFilePath << YAML::EndMap;
		auto texNode = YAML::Load(texEmitter.c_str());
		auto texDescriptor = std::make_unique<AssetMemoryDescriptor<Texture>>(texAssetId, texNode);
		descriptors.emplace_back(std::move(texDescriptor));
	}
	
	void AddSpritesheetDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors, const SpritesheetTiledSet& tileset) 
	{
		const std::string texFilePath = tileset.GetImageFilePath().string();
		std::string sptAssetId = GenerateAssetId(texFilePath + "_spt");

		YAML::Emitter sptEmitter;
		sptEmitter << YAML::BeginMap;
		sptEmitter << YAML::Key << "textureId" << YAML::Value << GenerateAssetId(texFilePath);
		sptEmitter << YAML::Key << "cols" << YAML::Value << tileset.GetColumns();
		sptEmitter << YAML::Key << "rows" << YAML::Value << tileset.GetRows();
		sptEmitter << YAML::Key << "margin" << YAML::Value << tileset.GetMargin();
		sptEmitter << YAML::Key << "spacing" << YAML::Value << tileset.GetSpacing();
		sptEmitter << YAML::EndMap;

		auto sptNode = YAML::Load(sptEmitter.c_str());
		auto sptDescriptor = std::make_unique<AssetMemoryDescriptor<Spritesheet>>(sptAssetId, sptNode);
		descriptors.emplace_back(std::move(sptDescriptor));

		mTileTextureLookupMap.emplace(tileset.GetFirstGid(), std::make_unique<SpritesheetData>(sptAssetId));
	}

	std::string GenerateAssetId(const std::string& filePath) 
	{
		std::hash<std::string> stringHasher;
		return std::to_string(stringHasher(filePath));
	}

private:
	std::unique_ptr<TiledMapData> mData;
	std::unordered_map<uint32_t, std::unique_ptr<TileTextureLookup>> mTileTextureLookupMap;
};