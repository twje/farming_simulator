#pragma once

#include <string_view>
#include <memory>
#include <filesystem>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward declaration
class TiledMapData;

namespace fs = std::filesystem;

class TiledMapJsonParser
{
public:
	static std::unique_ptr<TiledMapData> Load(const fs::path& filePath);

private:
	static std::unique_ptr<TiledMapData> LoadMapSettings(json& node, const fs::path& filePath);
	static void LoadLayers(TiledMapData& tiledMap, json& parentNode);
	static void LoadTileSets(const fs::path& directoryPath, TiledMapData& tiledMap, json& parentNode);

	// Utility Methods
	static void LoadJson(fs::path filePath, json& outJson);
	static uint32_t ExtractUInt32(const json& parentNode, const std::string& key);
	static std::string ExtractString(const json& parentNode, const std::string& key);
	static bool HasFileExtension(fs::path filePath, const std::string& extension);
};