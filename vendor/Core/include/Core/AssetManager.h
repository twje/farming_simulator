#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <filesystem>
#include <fstream>

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
	virtual std::unique_ptr<AssetBase> Load(const std::string& fileName, AssetManager& assetManager) = 0;
};

// ----------------------------------------------------------------
class AssetDescriptorBase
{
public:
	AssetDescriptorBase(const std::string& assetId, const std::string& fileName)
		: mAssetId(assetId),
		mFileName(fileName)
	{ }

	virtual ~AssetDescriptorBase() = default;

	const std::string& GetAssetId() const { return mAssetId; }
	const std::string& GetFileName() const { return mFileName; }
	virtual std::type_index GetAssetTypeId() const = 0;

private:
	std::string mAssetId;
	std::string mFileName;
};

// ----------------------------------------------------------------
template <typename T>
class AssetDescriptor : public AssetDescriptorBase
{
public:
	AssetDescriptor(const std::string& assetId, const std::string& fileName)
		: AssetDescriptorBase(assetId, fileName),
		mAssetTypeId(std::type_index(typeid(T)))
	{ }

	std::type_index GetAssetTypeId() const override { return mAssetTypeId; }

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
	T& GetAsset(const std::string assetId)
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
	void LoadAssetsFromManifest(std::string fileName)
	{
		namespace fs = std::filesystem;

		std::ifstream file(fileName);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to load configuration file " + fileName);
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
			std::string assetId, assetFileName;
			if (std::getline(iss >> std::ws, assetId, ',') && std::getline(iss >> std::ws, assetFileName, ','))
			{
				if (!fs::is_regular_file(assetFileName))
				{
					throw std::runtime_error("Resouce " + assetFileName + " does not exist");
				}

				QueueAssetDescriptor<T>(assetId, assetFileName);
			}
		}
	}

	void ProcessAssetQueue()
	{
		while (!mQueue.empty())
		{
			std::unique_ptr<AssetDescriptorBase> descriptor = std::move(mQueue.front());
			mQueue.pop();

			std::string fileName = descriptor->GetFileName();
			std::type_index assetTypeId = descriptor->GetAssetTypeId();
			std::string assetId = descriptor->GetAssetId();

			AssetRegistry& registry = GetAssetRegistry(assetTypeId);
			registry.LoadAsset(*this, assetId, fileName);
		}
	}

	template<typename T>
	T& GetAsset(const std::string& assetId)
	{
		AssetRegistry& registry = GetAssetRegistry(std::type_index(typeid(T)));
		return registry.GetAsset<T>(assetId);
	}

private:
	template<typename T>
	void QueueAssetDescriptor(const std::string& assetId, const std::string& fileName)
	{
		auto descriptor = std::make_unique<AssetDescriptor<T>>(assetId, fileName);
		mQueue.push(std::move(descriptor));
	}

	AssetRegistry& GetAssetRegistry(std::type_index assetTypeId)
	{
		auto it = mAssetRegistries.find(assetTypeId);
		assert(it != mAssetRegistries.end() && "Loader not registered");
		return it->second;
	}

private:
	std::unordered_map<std::type_index, AssetRegistry> mAssetRegistries;
	std::queue<std::unique_ptr<AssetDescriptorBase>> mQueue;
};