#pragma once

#include <memory>

#include "Core/ApplicationConfig.h"
#include "Core/AssetManager.h"

class ResourceLocator
{
	friend class Application;

public:
	ResourceLocator(ApplicationConfig config)
		: mConfig(config)
	{ }

	const ApplicationConfig& GetApplicationConfig() const { return mConfig; }
	AssetManager& GetAssetManager() { return mAssetManager; }

private:
	ApplicationConfig mConfig;
	AssetManager mAssetManager;
};