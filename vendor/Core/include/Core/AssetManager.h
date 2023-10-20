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
class AssetBase
{
public:
	virtual ~AssetBase() = default;
};

// ----------------------------------------------------------------
template<typename T>
class Asset : public AssetBase
{
public:
	Asset(std::unique_ptr<T> instance)
		: mInstance(std::move(instance))
	{ }

	T& GetInstance() { return *mInstance.get(); }

private:
	std::unique_ptr<T> mInstance;
};

// ----------------------------------------------------------------
class AssetLoader
{
public:
	virtual ~AssetLoader() = default;
	virtual std::unique_ptr<AssetBase> Load(const std::string& filePath, AssetManager& assetManager) = 0;
};

// ----------------------------------------------------------------
class AssetDescriptorBase
{
public:
	AssetDescriptorBase(const std::string& id, const std::string& filePath)
		: mfilePath(filePath),
		  mId(id)
	{ }

	virtual ~AssetDescriptorBase() = default;

	// Getters
	const std::string& GetFilePath() const { return mfilePath; }
	const std::string& GetId() const { return mId; }

	// Hook
	virtual std::type_index GetTypeId() const = 0;

private:
	std::string mfilePath;
	std::string mId;
};

// ----------------------------------------------------------------
template <typename T>
class AssetDescriptor : public AssetDescriptorBase
{
public:
	AssetDescriptor(const std::string& assetId, const std::string& filePath)
		: AssetDescriptorBase(assetId, filePath),
		  mAssetTypeId(std::type_index(typeid(T)))
	{ }

	std::type_index GetTypeId() const override { return mAssetTypeId; }

private:
	std::type_index mAssetTypeId;
};

// ----------------------------------------------------------------
class AssetRegistry
{
public:
	AssetRegistry(std::unique_ptr<AssetLoader> loader)
		: mLoader(std::move(loader))
	{ }

	void LoadAsset(AssetManager& manager, const std::string assetId, const std::string filePath)
	{
		assert(mAssets.find(assetId) == mAssets.end() && "Asset already loaded");
		auto asset = mLoader->Load(filePath, manager);
		mAssets.emplace(assetId, std::move(asset));
	}

	template<typename T>
	T& GetAsset(const std::string assetId) const
	{
		assert(mAssets.find(assetId) != mAssets.end() && "Asset not loaded");
		AssetBase* assetBase = mAssets.at(assetId).get();
		Asset<T>* asset = static_cast<Asset<T>*>(assetBase);
		return asset->GetInstance();
	}

private:
	std::unordered_map<std::string, std::unique_ptr<AssetBase>> mAssets;
	std::unique_ptr<AssetLoader> mLoader;
};

// ----------------------------------------------------------------
class AssetManager
{
public:
	AssetManager();

	template<typename T>
	void RegisterLoader(std::unique_ptr<AssetLoader> loader)
	{
		auto result = mAssetRegistries.emplace(std::type_index(typeid(T)), std::move(loader));
		assert(result.second && "Loader already registered");
	}

	template<typename T>
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

				QueueAssetDescriptor<T>(assetId, assetfilePath);
			}
		}
	}

	void ProcessAssetQueue()
	{
		while (!mQueue.empty())
		{
			std::unique_ptr<AssetDescriptorBase> desc = std::move(mQueue.front());
			mQueue.pop();

			AssetRegistry& registry = GetAssetRegistry(desc->GetTypeId());
			registry.LoadAsset(*this, desc->GetId(), desc->GetFilePath());
		}
	}

	template<typename T>
	T& GetAsset(const std::string& assetId) const
	{
		const AssetRegistry& registry = GetAssetRegistry(std::type_index(typeid(T)));
		return registry.GetAsset<T>(assetId);
	}

private:
	template<typename T>
	void QueueAssetDescriptor(const std::string& assetId, const std::string& filePath)
	{
		auto descriptor = std::make_unique<AssetDescriptor<T>>(assetId, filePath);
		mQueue.push(std::move(descriptor));
	}

	const AssetRegistry& GetAssetRegistry(std::type_index assetTypeId) const
	{
		auto it = mAssetRegistries.find(assetTypeId);
		assert(it != mAssetRegistries.end() && "Loader not registered");
		return it->second;
	}

	AssetRegistry& GetAssetRegistry(std::type_index assetTypeId)
	{
		return const_cast<AssetRegistry&>(std::as_const(*this).GetAssetRegistry(assetTypeId));
	}

private:
	std::unordered_map<std::type_index, AssetRegistry> mAssetRegistries;
	std::queue<std::unique_ptr<AssetDescriptorBase>> mQueue;
};