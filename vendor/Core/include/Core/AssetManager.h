#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <filesystem>
#include <fstream>
#include <utility>
#include <cassert>

#include "Core/TypeUtils.h"

// Forward declarations
class AssetManager;

// ----------------------------------------------------------------
class Asset
{
public:
	virtual ~Asset() = default;
	
	virtual void ResolveAssetDeps(AssetManager& assetManager)
	{
		ResolveAssetDepsImpl(assetManager);
		isAssetDepsResolved = true;
	}

private:	
	virtual void ResolveAssetDepsImpl(AssetManager& assetManager) = 0;

private:
	bool isAssetDepsResolved{ false };
};

// ----------------------------------------------------------------
class AssetLoader
{
public:
	virtual ~AssetLoader() = default;
	virtual std::unique_ptr<Asset> Load(const std::string& filePath) = 0;
};

// ----------------------------------------------------------------
class BaseAssetDescriptor
{
public:
	BaseAssetDescriptor(const std::string& id, const std::string& filePath)
		: mfilePath(filePath),
		  mId(id)
	{ }

	virtual ~BaseAssetDescriptor() = default;

	// Getters
	const std::string& GetFilePath() const { return mfilePath; }
	const std::string& GetId() const { return mId; }

	// Hook
	virtual uint32_t GetAssetTypeId() const = 0;

private:
	std::string mfilePath;
	std::string mId;
};

// ----------------------------------------------------------------
template <typename ASSET_TYPE>
class AssetDescriptor : public BaseAssetDescriptor
{
public:
	AssetDescriptor(const std::string& assetId, const std::string& filePath)
		: BaseAssetDescriptor(assetId, filePath),
		  mTypeId(TypeId<ASSET_TYPE>::Get())
	{ }

	uint32_t GetAssetTypeId() const override { return mTypeId; }

private:
	uint32_t mTypeId;
};

// ----------------------------------------------------------------
class AssetDescriptorQueue
{
public:		
	void Push(std::unique_ptr<BaseAssetDescriptor> descriptor)
	{		
		mQueue.push(std::move(descriptor));
	}

	std::unique_ptr<BaseAssetDescriptor> Pop()
	{
		std::unique_ptr<BaseAssetDescriptor> descriptor = std::move(mQueue.front());
		mQueue.pop();
		return descriptor;
	}

	bool IsEmpty() { return mQueue.empty(); }\

private:
	std::queue<std::unique_ptr<BaseAssetDescriptor>> mQueue;
};

// ----------------------------------------------------------------
class AssetRegistry
{
public:
	AssetRegistry(std::unique_ptr<AssetLoader> loader)
		: mLoader(std::move(loader))
	{ }

	Asset& LoadAsset(AssetManager& manager, const std::string assetId, const std::string filePath)
	{
		assert(mAssets.find(assetId) == mAssets.end() && "Asset already loaded");
		auto asset = mLoader->Load(filePath);
		mAssets.emplace(assetId, std::move(asset));

		return *mAssets[assetId].get();
	}

	template<typename ASSET_TYPE>
	ASSET_TYPE& GetAsset(const std::string& assetId) const
	{
		assert(mAssets.find(assetId) != mAssets.end() && "Asset not loaded");
		Asset* asset = mAssets.at(assetId).get();
		return *static_cast<ASSET_TYPE*>(asset);
	}

private:
	std::unordered_map<std::string, std::unique_ptr<Asset>> mAssets;
	std::unique_ptr<AssetLoader> mLoader;
};

// ----------------------------------------------------------------
class AssetManager
{
public:
	AssetManager();

	template<typename ASSET_TYPE>
	void RegisterLoader(std::unique_ptr<AssetLoader> loader)
	{
		auto result = mAssetRegistries.emplace(TypeId<ASSET_TYPE>::Get(), std::move(loader));
		assert(result.second && "Loader already registered");
	}

	template<typename ASSET_TYPE>
	void LoadAssetsFromManifest(std::string filePath)
	{
		namespace fs = std::filesystem;

		std::ifstream file(filePath);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to load configuration file " + filePath);
		}

		std::string line;
		while (std::getline(file, line))
		{
			auto isSpace = [](char c) { return std::isspace(c); };
			if (line.empty() || std::all_of(line.begin(), line.end(), isSpace) || line[0] == '#')
			{
				continue;
			}

			std::istringstream iss(line);
			std::string assetId, assetfilePath;
			if (std::getline(iss >> std::ws, assetId, ',') && std::getline(iss >> std::ws, assetfilePath, ','))
			{
				if (!fs::is_regular_file(assetfilePath))
				{
					throw std::runtime_error("Resouce " + assetfilePath + " does not exist");
				}

				auto descriptor = std::make_unique<AssetDescriptor<ASSET_TYPE>>(assetId, assetfilePath);
				mQueue.Push(std::move(descriptor));
			}
		}
	}

	void ProcessAssetQueue()
	{
		while (!mQueue.IsEmpty())
		{
			std::unique_ptr<BaseAssetDescriptor> desc = mQueue.Pop();			
			AssetRegistry& registry = GetAssetRegistry(desc->GetAssetTypeId());
			Asset& asset = registry.LoadAsset(*this, desc->GetId(), desc->GetFilePath());
			
			// TODO: validate dependencies
			asset.ResolveAssetDeps(*this);
		}
	}

	template<typename T>
	T& GetAsset(const std::string& assetId) const
	{
		const AssetRegistry& registry = GetAssetRegistry(TypeId<T>::Get());
		return registry.GetAsset<T>(assetId);
	}

private:
	const AssetRegistry& GetAssetRegistry(uint32_t assetTypeId) const
	{
		auto it = mAssetRegistries.find(assetTypeId);
		assert(it != mAssetRegistries.end() && "Loader not registered");
		return it->second;
	}

	AssetRegistry& GetAssetRegistry(std::uint32_t assetTypeId)
	{
		return const_cast<AssetRegistry&>(std::as_const(*this).GetAssetRegistry(assetTypeId));
	}

private:
	std::unordered_map<uint32_t, AssetRegistry> mAssetRegistries;
	AssetDescriptorQueue mQueue;
};