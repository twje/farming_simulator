#pragma once

#include <memory>
#include <unordered_map>
#include <queue>
#include <filesystem>
#include <fstream>
#include <utility>
#include <cassert>

#include "Core/TypeUtils.h"

// Forward declarations
// ----------------------------------------------------------------
template <typename ASSET_TYPE>
class AssetFileDescriptor;
template <typename ASSET_TYPE>
class AssetMemoryDescriptor;
class AssetManager;
class BaseAssetDescriptor;

// ----------------------------------------------------------------
class Asset
{
public:
	virtual ~Asset() = default;
	virtual void ResolveAssetDeps(AssetManager& assetManager) { };
	virtual std::vector<std::unique_ptr<BaseAssetDescriptor>> GetDependencyDescriptors() { return {}; }
};

// ----------------------------------------------------------------
class BaseAssetLoader
{
public:
	virtual ~BaseAssetLoader() = default;
};

// ----------------------------------------------------------------
template<typename ASSET_TYPE>
class AssetLoader : public BaseAssetLoader
{
public:	
	virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<ASSET_TYPE> descriptor)
	{
		assert(false && "Loader method for type AssetDescriptor not implemented");
		return nullptr;
	}

	virtual std::unique_ptr<Asset> Load(AssetMemoryDescriptor<ASSET_TYPE> descriptor)
	{
		assert(false && "Loader method for type AssetDescriptor not implemented");
		return nullptr;
	}
};

// ----------------------------------------------------------------
class BaseAssetDescriptor
{
public:
	explicit BaseAssetDescriptor(const std::string& assetId)
		: mAssetId(assetId)
	{ }

	virtual ~BaseAssetDescriptor() = default;

	// Hooks
	virtual std::unique_ptr<Asset> LoadAsset(BaseAssetLoader& loader) const = 0;
	virtual uint32_t GetAssetTypeId() const = 0;

	// Getter
	const std::string& GetAssetId() const { return mAssetId; }

private:
	std::string mAssetId;
};

// ----------------------------------------------------------------
template <typename ASSET_TYPE>
class AssetFileDescriptor : public BaseAssetDescriptor
{
public:
	AssetFileDescriptor(const std::string& assetId, const std::string& filePath)
		: BaseAssetDescriptor(assetId)		  
		, mfilePath(filePath)
	{ }

	// BaseAssetDescriptor interface
	virtual std::unique_ptr<Asset> LoadAsset(BaseAssetLoader& loader) const override
	{		
		return static_cast<AssetLoader<ASSET_TYPE>&>(loader).Load(*this);
	}
	
	uint32_t GetAssetTypeId() const override { return TypeId<ASSET_TYPE>::Get(); }

	// Getters
	const std::string& GetFilePath() const { return mfilePath; }

private:
	std::string mfilePath;	
};

// ----------------------------------------------------------------
template <typename ASSET_TYPE>
class AssetMemoryDescriptor : public BaseAssetDescriptor
{
public:
	AssetMemoryDescriptor(const std::string& assetId)
		: BaseAssetDescriptor(assetId)
	{ }

	// BaseAssetDescriptor interface
	virtual std::unique_ptr<Asset> LoadAsset(BaseAssetLoader& loader) const override
	{
		return static_cast<AssetLoader<ASSET_TYPE>&>(loader).Load(*this);
	}

	uint32_t GetAssetTypeId() const override { return TypeId<ASSET_TYPE>::Get(); }
};


// ----------------------------------------------------------------
class AssetDescriptorQueue
{
public:
	void Push(std::vector<std::unique_ptr<BaseAssetDescriptor>>&& dependencyDescriptors)
	{
		for (auto& descriptor : dependencyDescriptors)
		{
			Push(std::move(descriptor));
		}
	}

	void Push(std::unique_ptr<BaseAssetDescriptor> descriptor)
	{		
		mQueue.push(std::move(descriptor));
	}

	std::unique_ptr<BaseAssetDescriptor> Pop()
	{
		auto descriptor = std::move(mQueue.front());
		mQueue.pop();
		return descriptor;
	}

	bool IsEmpty() const { return mQueue.empty(); }

private:
	std::queue<std::unique_ptr<BaseAssetDescriptor>> mQueue;
};

// ----------------------------------------------------------------
class AssetRegistry
{
public:
	AssetRegistry(std::unique_ptr<BaseAssetLoader> loader)
		: mLoader(std::move(loader))
	{ }
	
	Asset& LoadAsset(BaseAssetDescriptor& descriptor)
	{
		assert(mAssets.find(descriptor.GetAssetId()) == mAssets.end() && "Asset already loaded");
		auto asset = descriptor.LoadAsset(*mLoader);		
		mAssets.emplace(descriptor.GetAssetId(), std::move(asset));

		return *mAssets[descriptor.GetAssetId()].get();
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
	std::unique_ptr<BaseAssetLoader> mLoader;
};

// ----------------------------------------------------------------
class AssetManager
{
public:
	AssetManager();

	template<typename ASSET_TYPE>
	void RegisterLoader(std::unique_ptr<BaseAssetLoader> loader)
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

				auto descriptor = std::make_unique<AssetFileDescriptor<ASSET_TYPE>>(assetId, assetfilePath);
				mQueue.Push(std::move(descriptor));
			}
		}
	}

	void ProcessAssetQueue()
	{
		std::vector<Asset*> loadedAssets;
		while (!mQueue.IsEmpty())
		{
			std::unique_ptr<BaseAssetDescriptor> descriptor = mQueue.Pop();			
			
			AssetRegistry& registry = GetAssetRegistry(descriptor->GetAssetTypeId());
			Asset* asset = &registry.LoadAsset(*descriptor);
			
			auto&& dependencyDescriptors = asset->GetDependencyDescriptors();
			mQueue.Push(std::move(dependencyDescriptors));

			loadedAssets.push_back(asset);			
		}

		for (Asset* asset : loadedAssets)
		{
			asset->ResolveAssetDeps(*this);
		}
	}

	template<typename ASSET_TYPE>
	ASSET_TYPE& GetAsset(const std::string& assetId) const
	{
		const AssetRegistry& registry = GetAssetRegistry(TypeId<ASSET_TYPE>::Get());
		return registry.GetAsset<ASSET_TYPE>(assetId);
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