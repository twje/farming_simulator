#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <filesystem>

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
class AssetManager
{
	using AssetLookupMap = std::unordered_map<std::string, std::unique_ptr<AssetBase>>;
	using AssetMap = std::unordered_map<std::type_index, AssetLookupMap>;
	using AssetLoaderMap = std::unordered_map<std::type_index, std::unique_ptr<AssetLoader>>;
	using AssetQueue = std::queue<std::unique_ptr<AssetDescriptorBase>>;

public:
	AssetManager();

	template<typename T>
	void RegisterLoader(std::unique_ptr<AssetLoader> loader)
	{
		mLoaders.emplace(std::type_index(typeid(T)), std::move(loader));
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

			std::type_index assetTypeId = descriptor->GetAssetTypeId();
			std::string assetId = descriptor->GetAssetId();
			std::string fileName = descriptor->GetFileName();

			if (IsAssetLoaded(assetTypeId, assetId))
			{
				throw std::runtime_error("Asset is already loaded");
			}

			AssetLoader& loader = GetLoader(assetTypeId);
			auto asset = loader.Load(fileName, *this);
			mAssets[assetTypeId].emplace(assetId, std::move(asset));
		}
	}

	template<typename T>
	T& GetAsset(const std::string& assetId) const
	{
		std::type_index assetTypeId = std::type_index(typeid(T));
		if (!IsAssetLoaded(assetTypeId, assetId))
		{
			throw std::runtime_error("Asset not loaded");
		}

		auto assetLookupIt = mAssets.at(assetTypeId).find(assetId);
		AssetBase* assetBase = assetLookupIt->second.get();
		Asset<T>* asset = dynamic_cast<Asset<T>*>(assetBase);
		return asset->GetInstance();
	}

private:
	template<typename T>
	void QueueAssetDescriptor(const std::string& assetId, const std::string& fileName)
	{
		auto descriptor = std::make_unique<AssetDescriptor<T>>(assetId, fileName);
		mQueue.push(std::move(descriptor));
	}

	AssetLoader& GetLoader(std::type_index assetTypeId) const
	{
		auto loaderIt = mLoaders.find(assetTypeId);
		if (loaderIt == mLoaders.end())
		{
			throw std::runtime_error("Loader not found for the asset type");
		}
		return *loaderIt->second.get();
	}

	bool IsAssetLoaded(std::type_index assetTypeId, const std::string& assetId) const
	{
		auto assetIt = mAssets.find(assetTypeId);
		if (assetIt == mAssets.end())
		{
			return false;
		}

		auto assetLookupIt = mAssets.at(assetTypeId).find(assetId);
		if (assetLookupIt == mAssets.at(assetTypeId).end())
		{
			return false;
		}

		return true;
	}

private:
	AssetMap mAssets;
	AssetLoaderMap mLoaders;
	AssetQueue mQueue;
};