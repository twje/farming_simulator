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
		for (const TiledSet& tileset : mData->GetTiledSets()) 
		{
			AddTextureDescriptor(descriptors, tileset);
			AddSpritesheetDescriptor(descriptors, tileset);
		}
		return descriptors;
	}	

	void Draw(sf::RenderWindow& window) 
	{
		for (const TiledLayer& layer : mData->GetTiledLayers()) {
			DrawLayer(window, layer);
		}
	}

private:
	void DrawLayer(sf::RenderWindow& window, const TiledLayer& layer) 
	{
		uint32_t tileWidth = mData->GetTileWidth();
		uint32_t tileHeight = mData->GetTileHeight();
		
		for (size_t y = 0; y < layer.GetHeight(); y++) 
		{
			for (size_t x = 0; x < layer.GetWidth(); x++) 
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

		const TiledSet& tiledSet = mData->GetTileSet(globalTileId);
		const SpritesheetData& spritesheetData = mSpritesheetDataMap.at(tiledSet.GetFirstGid());
		const Spritesheet& spritesheet = spritesheetData.GetSpritesheet();

		uint32_t localTileId = globalTileId - tiledSet.GetFirstGid();
		const TextureRegion& textureRegion = spritesheet.GetTextureRegion(localTileId);

		sf::Sprite sprite(*textureRegion.GetTexture(), textureRegion.GetRegion());
		sprite.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
		window.draw(sprite);
	}

	void AddTextureDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors, const TiledSet& tileset) 
	{
		const std::string texFilePath = tileset.GetImageFilePath().string();
		std::string texAssetId = GenerateAssetId(texFilePath);

		YAML::Emitter texEmitter;
		texEmitter << YAML::BeginMap << YAML::Key << "filePath" << YAML::Value << texFilePath << YAML::EndMap;
		auto texNode = YAML::Load(texEmitter.c_str());
		auto texDescriptor = std::make_unique<AssetMemoryDescriptor<Texture>>(texAssetId, texNode);
		descriptors.emplace_back(std::move(texDescriptor));
	}
	
	void AddSpritesheetDescriptor(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors, const TiledSet& tileset) 
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