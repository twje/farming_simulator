#pragma once

#include <string_view>
#include <memory>
#include <filesystem>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward declaration
class TiledMap;

namespace fs = std::filesystem;

class TiledMapLoader
{
public:
	static std::unique_ptr<TiledMap> Load(const fs::path& filePath);

private:
	static std::unique_ptr<TiledMap> LoadMapSettings(json& node);
	static void LoadLayers(TiledMap& tiledMap, json& parentNode);
	static void LoadTileSets(const fs::path& directoryPath, TiledMap& tiledMap, json& parentNode);

	// Utility Methods
	static void LoadJson(fs::path filePath, json& outJson);
	static uint32_t ExtractUInt32(const json& parentNode, const std::string& key);
	static std::string ExtractString(const json& parentNode, const std::string& key);
	static bool HasFileExtension(fs::path filePath, const std::string& extension);
};