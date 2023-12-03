#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/AssetManager.h"
#include "Core/Tiled/TiledMapData.h"
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
class SpritesheetData
{
public:	
	SpritesheetData(std::string id, Spritesheet* sheet)
		: mAssetId(std::move(id)), mSpritesheet(sheet) 
	{ }
	
	void SetAssetId(const std::string& id)  { mAssetId = id; }
	void SetSpritesheet(Spritesheet* sheet) { mSpritesheet = sheet; }

	const std::string& GetAssetId() const { return mAssetId; }
	Spritesheet& GetSpritesheet() const { return *mSpritesheet; }

private:
	std::string mAssetId;
	Spritesheet* mSpritesheet{ nullptr };
};

//------------------------------------------------------------------------------
class TiledMap : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMap(std::unique_ptr<TiledMapData> data)
		: mData(std::move(data))
	{ }

	virtual void ResolveAssetDeps(AssetManager& assetManager)
	{
		for (auto& pair : mSpritesheetDataMap)
		{
			SpritesheetData& spritesheetData = pair.second;
			Spritesheet* spritesheet = &assetManager.GetAsset<Spritesheet>(spritesheetData.GetAssetId());
			spritesheetData.SetSpritesheet(spritesheet);
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
		return descriptors;
	}	

	void Draw(sf::RenderWindow& window, const sf::IntRect& region)
	{
		for (const TiledLayer& layer : mData->GetTiledLayers())
		{
			DrawLayer(window, layer, region);
		}
	}

private:
	int32_t Clamp(int32_t minValue, int32_t maxValue, int32_t value)
	{
		return std::min(maxValue, std::max(minValue, value));
	}

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

		const SpritesheetTiledSet& tiledSet = mData->GetmSpritesheetTiledSet(globalTileId);
		const SpritesheetData& spritesheetData = mSpritesheetDataMap.at(tiledSet.GetFirstGid());
		const Spritesheet& spritesheet = spritesheetData.GetSpritesheet();

		uint32_t localTileId = globalTileId - tiledSet.GetFirstGid();
		const TextureRegion& textureRegion = spritesheet.GetTextureRegion(localTileId);

		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		window.draw(sprite);
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

		mSpritesheetDataMap.emplace(tileset.GetFirstGid(), SpritesheetData(sptAssetId, nullptr));
	}

	std::string GenerateAssetId(const std::string& filePath) 
	{
		std::hash<std::string> stringHasher;
		return std::to_string(stringHasher(filePath));
	}

private:
	std::unique_ptr<TiledMapData> mData;
	std::unordered_map<uint32_t, SpritesheetData> mSpritesheetDataMap;
};