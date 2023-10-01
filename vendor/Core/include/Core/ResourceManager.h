#pragma once

#include <memory>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

// Forward declaration
class ResourceLocator;

template<typename T>
class ResourceLoader
{
public:
	virtual std::unique_ptr<T> Load(std::string filepath, ResourceLocator& locator) = 0;
};

template<typename T>
class ResourceManager
{
public:
	ResourceManager(std::unique_ptr<ResourceLoader<T>> loader, ResourceLocator& locator)
		: mLoader(std::move(loader)),
          mLocator(locator)
	{}

	void LoadConfig(std::string filePath)
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
            std::string resourceId, resourcePath;
            if (std::getline(iss >> std::ws, resourceId, ',') && std::getline(iss >> std::ws, resourcePath, ','))
            {
                if (!fs::is_regular_file(resourcePath))
                {
                    throw std::runtime_error("Resouce " + resourcePath + " does not exist");
                }
                mResources[resourceId] = mLoader->Load(resourcePath, mLocator);
            }
        }
	}

	T& Get(std::string resourceId)
	{
        if (mResources.find(resourceId) == mResources.end())
        {
            throw std::runtime_error("resourceId does not exist: " + resourceId);
        }
        return *mResources.at(resourceId);
	}

	void Clone(std::string resourceId)
	{
		return Get(resourceId).Clone();
	}

private:
	std::map<std::string, std::unique_ptr<T>> mResources;
	std::unique_ptr<ResourceLoader<T>> mLoader;
    ResourceLocator& mLocator;
};
