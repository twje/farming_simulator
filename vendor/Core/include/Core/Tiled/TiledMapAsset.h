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

//------------------------------------------------------------------------------
class TileTextureResolver
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
class SpritesheetTextureProvider : public TileTextureResolver
{
public:
	SpritesheetTextureProvider(const SpritesheetTiledSet& tileset)
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
class IndividualTextureProvider : public TileTextureResolver
{
public:
	IndividualTextureProvider(const ImageCollectionTiledSet& tileset)
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
class TiledTextureManager : public TiledMapElementVisitor
{
	using TiledSetRef = std::reference_wrapper<const TiledSet>;	

public:
	void GetDependencyDescriptors(const std::vector<TiledSetRef>& tilesets, std::vector<std::unique_ptr<BaseAssetDescriptor>>& outDescriptors)
	{
		for (const TiledSet& tileset : tilesets)
		{
			tileset.Visit(*this);
		}

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

	TextureRegion GetTextureRegion(uint32_t firstGid, uint32_t gid) const
	{
		uint32_t localTileId = gid - firstGid;
		const TileTextureResolver* resolver = mResourceMap.at(firstGid).get();
		return resolver->GetTextureRegion(localTileId);
	}

private:
	virtual void Accept(const SpritesheetTiledSet& element) override
	{
		auto provider = std::make_unique<SpritesheetTextureProvider>(element);
		mResourceMap.emplace(element.GetFirstGid(), std::move(provider));
	}
	
	virtual void Accept(const ImageCollectionTiledSet& element) override
	{
		auto provider = std::make_unique<IndividualTextureProvider>(element);
		mResourceMap.emplace(element.GetFirstGid(), std::move(provider));
	}

private:
	std::unordered_map<uint32_t, std::unique_ptr<TileTextureResolver>> mResourceMap;
};

//------------------------------------------------------------------------------
class TiledMapAsset : public Asset, private NonCopyableNonMovableMarker
{
public:
	TiledMapAsset(std::unique_ptr<TiledMapData> data)
		: mData(std::move(data))		
	{ }

	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors() override
	{		
		std::vector<std::unique_ptr<BaseAssetDescriptor>> descriptors;
		mTiledTextureManager.GetDependencyDescriptors(mData->GetTiledSets(), descriptors);
		return descriptors;
	}

	virtual void ResolveAssetDeps(AssetManager& assetManager) override
	{
		mTiledTextureManager.ResolveDependencies(assetManager);
	}

	TextureRegion GetTextureRegion(uint32_t globalTileId) const
	{
		const TiledSet& tiledSet = mData->GetTiledSet(globalTileId);
		return mTiledTextureManager.GetTextureRegion(tiledSet.GetFirstGid(), globalTileId);
	}

	// Getters
	uint32_t GetTileWidth() const { return mData->GetTileWidth(); }
	uint32_t GetTileHeight() const { return mData->GetTileHeight(); }
	uint32_t GetMapWidth() const { return mData->GetMapWidth(); }
	uint32_t GetMapHeight() const { return mData->GetMapHeight(); }
	const std::vector<std::unique_ptr<Layer>>& GetTiledLayers() { return mData->GetTiledLayers(); }

private:
	std::unique_ptr<TiledMapData> mData;
	TiledTextureManager mTiledTextureManager;
};