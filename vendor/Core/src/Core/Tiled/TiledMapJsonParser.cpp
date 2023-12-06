#include "Core/Tiled/TiledMapJsonParser.h"

#include <fstream>
#include <iostream>
#include <cassert>

#include "Core/Tiled/TiledMapData.h"

// --------------------------------------------------------------------------------
/*static*/ std::unique_ptr<TiledMapData> TiledMapJsonParser::Load(const fs::path& filePath)
{
    assert(HasFileExtension(filePath, ".json"));

    json parentNode;
    LoadJson(filePath.string(), parentNode);

    std::unique_ptr<TiledMapData> tiledMap = LoadMapSettings(parentNode, filePath);
    LoadLayers(*tiledMap.get(), parentNode);
    LoadTileSets(filePath.parent_path(), *tiledMap.get(), parentNode);

	return tiledMap;
}

// --------------------------------------------------------------------------------
/*static*/ std::unique_ptr<TiledMapData> TiledMapJsonParser::LoadMapSettings(json& parentNode, const fs::path& filePath)
{
    uint32_t width = ExtractUInt32(parentNode, "width");
    uint32_t height = ExtractUInt32(parentNode, "height");
    uint32_t tileWidth = ExtractUInt32(parentNode, "tilewidth");
    uint32_t tileHeight = ExtractUInt32(parentNode, "tileheight");

    return std::make_unique<TiledMapData>(filePath, width, height, tileWidth, tileHeight);
}

// --------------------------------------------------------------------------------
/*static*/ void TiledMapJsonParser::LoadLayers(TiledMapData& tiledMap, json& parentNode)
{
    for (const json& layerNode : parentNode["layers"])
    {
        uint32_t layerId = ExtractUInt32(layerNode, "id");
        const std::string& layerName = ExtractString(layerNode,"name");
        const std::string& layerType = ExtractString(layerNode, "type");
        if (layerType == "tilelayer")
        {
            uint32_t layerWidth = ExtractUInt32(layerNode, "width");
            uint32_t layerHeight = ExtractUInt32(layerNode, "height");

            std::vector<uint32_t> tiles;
            for (const json& tileNode : layerNode["data"])
            {
                tiles.emplace_back(tileNode.get<uint32_t>());
            }

            TiledLayer layer(layerId, layerName, layerWidth, layerHeight, std::move(tiles));
            tiledMap.AddLayer(std::move(layer));
        }
    }
}

// --------------------------------------------------------------------------------
/*static*/ void TiledMapJsonParser::LoadTileSets(const fs::path& directoryPath, TiledMapData& tiledMap, json& parentNode)
{
    for (json tilesetNode : parentNode["tilesets"])
    {
        fs::path relativeTilesetDirectory = directoryPath;

        // Parse data
        json tilesetData;
        if (tilesetNode.contains("source"))
        {
            const std::string& sourcefilePath = ExtractString(tilesetNode, "source");
            assert(HasFileExtension(sourcefilePath, ".json"));            
            fs::path filePath = directoryPath / fs::path(sourcefilePath);
            LoadJson(filePath, tilesetData);

            relativeTilesetDirectory = filePath.parent_path();
        }
        else
        {
            tilesetData = tilesetNode;
        }

        uint32_t columns = ExtractUInt32(tilesetData, "columns");                

        TiledSetData tiledSetData(
            ExtractString(tilesetData, "name"),
            ExtractUInt32(tilesetNode, "firstgid"),     // firstgid is always present in map file
            ExtractUInt32(tilesetData, "tilewidth"),
            ExtractUInt32(tilesetData, "tileheight"),
            columns,
            ExtractUInt32(tilesetData, "margin"),
            ExtractUInt32(tilesetData, "spacing"),
            ExtractUInt32(tilesetData, "tilecount")
        );
        
        // Spritesheet TiledSet
        if (columns > 0)
        {
            fs::path relativeImageFilePath(ExtractString(tilesetData, "image"));
            fs::path absoluteImageFilePath = fs::absolute(relativeTilesetDirectory / relativeImageFilePath);

            SpritesheetTiledSet tiledSet(
                std::move(tiledSetData),
                absoluteImageFilePath, 
                ExtractUInt32(tilesetData, "imageheight"),
                ExtractUInt32(tilesetData, "imagewidth")
            );

            tiledMap.AddSpritesheetTiledSet(std::move(tiledSet));
        }
        // Image Collection TiledSet
        else
        {
            std::vector<ImageTile> imageTiles;
            for (const json& tileNode : tilesetData["tiles"])
            {
                fs::path relativeImageFilePath(ExtractString(tileNode, "image"));
                fs::path absoluteImageFilePath = fs::absolute(relativeTilesetDirectory / relativeImageFilePath);

                ImageTile tile(
                    ExtractUInt32(tileNode, "id"),
                    absoluteImageFilePath,
                    ExtractUInt32(tileNode, "imageheight"),
                    ExtractUInt32(tileNode, "imagewidth")
                );
                imageTiles.emplace_back(tile);
            }

            ImageCollectionTiledSet tiledSet(std::move(tiledSetData), std::move(imageTiles));
            tiledMap.AddImageCollectionTiledSet(std::move(tiledSet));
        }
    }
}

// --------------------------------------------------------------------------------
/*static*/ void TiledMapJsonParser::LoadJson(fs::path filePath, json& outJson)
{
    std::ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        throw std::runtime_error("Error opening file: " + filePath.string());
    }
    try
    {
        ifs >> outJson;
    }
    catch (const json::parse_error& e)
    {
        throw std::runtime_error("Error parsing JSON: " + std::string(e.what()));
    }
}

// --------------------------------------------------------------------------------
/*static*/ uint32_t TiledMapJsonParser::ExtractUInt32(const json& parentNode, const std::string& key)
{
    if (!parentNode.contains(key))
    {
        throw std::invalid_argument("Invalid JSON structure: missing key - " + key);
    }
    return parentNode[key].get<uint32_t>();
}

// --------------------------------------------------------------------------------
/*static*/ std::string TiledMapJsonParser::ExtractString(const json& parentNode, const std::string& key)
{
    if (!parentNode.contains(key))
    {
        throw std::invalid_argument("Invalid JSON structure: missing key - " + key);
    }
    return parentNode[key].get<std::string>();
}

// --------------------------------------------------------------------------------
/*static*/ bool TiledMapJsonParser::HasFileExtension(fs::path filePath, const std::string& extension)
{
    if (filePath.has_extension())
    {
        return filePath.extension() == extension;
    }
    return false;
}