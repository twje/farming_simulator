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
#include <iostream>

//------------------------------------------------------------------------------
class TileTextureResolver
{
public:
	// Dependency management
	virtual void GetDependencyDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors) = 0;
	virtual void ResolveDependencies(AssetManager& assetManager) = 0;
	
	virtual const TextureRegion& GetTextureRegion(uint32_t index) const = 0;
	virtual uint32_t GetFirstGid() const = 0;

protected:
	std::string GenerateAssetId(const std::string& filePath)
	{
		std::hash<std::string> stringHasher;
		return std::to_string(stringHasher(filePath));
	}
};

//------------------------------------------------------------------------------
class SpritesheetTextureProvider : public TileTextureResolver
{
public:
	SpritesheetTextureProvider(const SpritesheetTilesetData& tileset)
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

	virtual const TextureRegion& GetTextureRegion(uint32_t index) const override
	{
		return mSpritesheet.second->GetTextureRegion(index);
	}

	virtual uint32_t GetFirstGid() const override { return mTileset.GetFirstGid(); }

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
	const SpritesheetTilesetData& mTileset;
	std::pair<std::string, Spritesheet*> mSpritesheet;	
};

//------------------------------------------------------------------------------
class IndividualTextureProvider : public TileTextureResolver
{
public:
	IndividualTextureProvider(const ImageCollectionTilesetData& tileset)
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

	const TextureRegion& GetTextureRegion(uint32_t index) const override
	{
		sf::Texture* texture = &mTextures[index].second->GetRawTexture();
		sf::IntRect region(sf::Vector2i(), sf::Vector2i(texture->getSize()));

		mTextureRegion.SetRegion(region);
		mTextureRegion.SetTexture(texture);

		return mTextureRegion;
	}

	virtual uint32_t GetFirstGid() const override { return mTileset.GetFirstGid(); }

private:
	void AddTextureDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& descriptors)
	{		
		for (const TilesetImageData& imageTile : mTileset.GetImageTiles())
		{
			const std::string texFilePath = imageTile.GetImageFilePath().string();
			std::string texAssetId = GenerateAssetId(texFilePath);
			std::cout << texAssetId << std::endl;

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
	mutable TextureRegion mTextureRegion;
	const ImageCollectionTilesetData& mTileset;
	std::vector<std::pair<std::string, Texture*>> mTextures;
};

//------------------------------------------------------------------------------
class TiledTextureManager
{
public:
	void AddTextureProvider(std::unique_ptr<TileTextureResolver> provider)
	{
		mResourceMap.emplace(provider->GetFirstGid(), std::move(provider));
	}

	void GetDependencyDescriptors(std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors)
	{
		for (const auto& pair : mResourceMap)
		{
			pair.second->GetDependencyDescriptors(outDescriptors);
		}
	}

	void ResolveDependencies(AssetManager& assetManager)
	{
		for (auto& pair : mResourceMap)
		{
			TileTextureResolver* provider = pair.second.get();
			provider->ResolveDependencies(assetManager);
		}
	}

	const TextureRegion& GetTextureRegion(uint32_t globalTileId) const
	{
		TileTextureResolver* resolver = GetResolver(globalTileId);		
		uint32_t localTileId = globalTileId - resolver->GetFirstGid();		
		return resolver->GetTextureRegion(localTileId);
	}

private:
	TileTextureResolver* GetResolver(uint32_t globalTileId) const
	{
		TileTextureResolver* resolver = nullptr;

		uint32_t closestValue = std::numeric_limits<uint32_t>::max();
		for (const auto& pair : mResourceMap)
		{
			if (globalTileId >= pair.first)
			{
				uint32_t distance = globalTileId - pair.first;
				if (closestValue >= distance)
				{
					closestValue = distance;
					resolver = pair.second.get();
				}
			}
		}	

		assert(resolver);
		return resolver;
	}

private:
	std::unordered_map<uint32_t, std::unique_ptr<TileTextureResolver>> mResourceMap;
};

//------------------------------------------------------------------------------
class TiledMapAsset : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMapAsset(std::unique_ptr<TiledMapData> tiledMapData)
		: mTiledMapData(std::move(tiledMapData))
	{ 
		for (const SpritesheetTilesetData& tileset : mTiledMapData->GetSpritesheetTilesets())
		{
			auto provider = std::make_unique<SpritesheetTextureProvider>(tileset);
			mTiledTextureManager.AddTextureProvider(std::move(provider));
		}

		for (const ImageCollectionTilesetData& tileset : mTiledMapData->GetImageCollectionTilesets())
		{
			auto provider = std::make_unique<IndividualTextureProvider>(tileset);
			mTiledTextureManager.AddTextureProvider(std::move(provider));
		}
	}

	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors() override
	{		
		std::vector<std::unique_ptr<BaseAssetDescriptor>> descriptors;
		mTiledTextureManager.GetDependencyDescriptors(descriptors);
		return descriptors;
	}

	virtual void ResolveAssetDeps(AssetManager& assetManager) override
	{
		mTiledTextureManager.ResolveDependencies(assetManager);
	}

	const TextureRegion& GetTextureRegion(uint32_t globalTileId) const
	{		
		return mTiledTextureManager.GetTextureRegion(globalTileId);
	}

	// Map data
	uint32_t GetTileWidth() const { return mTiledMapData->GetTileWidth(); }
	uint32_t GetTileHeight() const { return mTiledMapData->GetTileHeight(); }
	uint32_t GetMapWidth() const { return mTiledMapData->GetMapWidth(); }
	uint32_t GetMapHeight() const { return mTiledMapData->GetMapHeight(); }
	
	// Layers
	const std::vector<TileLayerData>& GetTileLayers() { return mTiledMapData->GetTileLayers(); }
	const std::vector<ObjectLayerData>& GetObjectLayers() const { return mTiledMapData->GetObjectLayers(); }

private:	
	std::unique_ptr<TiledMapData> mTiledMapData;
	TiledTextureManager mTiledTextureManager;
};