#pragma once

#include <memory>

#include "Core/ApplicationConfig.h"
#include "Core/AssetManager.h"

class ResourceLocator
{
	friend class Application;

public:
	ResourceLocator(const ResourceLocator&) = delete;
	ResourceLocator& operator=(const ResourceLocator&) = delete;

	static ResourceLocator& GetInstance() 
	{
		static ResourceLocator instance;
		return instance;
	}

	void Initialize(const ApplicationConfig& config) 
	{
		mConfig = config;
	}	

	const ApplicationConfig& GetApplicationConfig() const { return mConfig; }
	ApplicationConfig& GetApplicationConfig() { return mConfig; }

	AssetManager& GetAssetManager() { return mAssetManager; }

private:
	ResourceLocator() = default;

	ApplicationConfig mConfig;
	AssetManager mAssetManager;
};