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
    uint32_t depth = 0;
    for (const json& layerNode : parentNode["layers"])
    {
        LayerAttributes layerAttributes(ExtractUInt32(layerNode, "id"), 
                                        ExtractString(layerNode, "name"),
                                        ExtractBool(layerNode, "visible"),
                                        depth++);

        const std::string& layerType = ExtractString(layerNode, "type");
        if (layerType == "tilelayer")
        {
            std::vector<TileData> tiles;
            for (const json& tileNode : layerNode["data"])
            {
                tiles.emplace_back(tileNode.get<uint32_t>());
            }
            
            TileLayerData layer(std::move(layerAttributes),
                                ExtractUInt32(layerNode, "width"),
                                ExtractUInt32(layerNode, "height"),
                                std::move(tiles));            

            tiledMap.AddTileLayer(std::move(layer));
        }
        else if (layerType == "objectgroup")
        {         
            std::vector<ObjectData> objects;
            for (const json& objectNode : layerNode["objects"])
            {
                ObjectAttributes objectAttributes(ExtractString(objectNode, "name"),
                                                  ExtractUInt32(objectNode, "id"),
                                                  ExtractUInt32(objectNode, "width"),
                                                  ExtractUInt32(objectNode, "height"),
                                                  ExtractUInt32(objectNode, "rotation"),
                                                  ExtractBool(objectNode, "visible"),
                                                  ExtractInt32(objectNode, "x"),
                                                  ExtractInt32(objectNode, "y"));

                if (objectNode.find("point") != objectNode.end())
                {
                    objects.emplace_back(std::move(objectAttributes), ObjectType::POINT);
                }
                else if (objectNode.find("ellipse") != objectNode.end())
                {
                    objects.emplace_back(std::move(objectAttributes), ObjectType::ELLIPSE);
                }
                else if (objectNode.find("polyline") != objectNode.end())
                {
                    objects.emplace_back(std::move(objectAttributes), ObjectType::POLYGON);
                }
                else if (objectNode.find("gid") != objectNode.end())
                {
                    objects.emplace_back(std::move(objectAttributes),
                                         ObjectType::TILE,
                                         ExtractUInt32(objectNode, "gid"));
                }
                else
                {                    
                    objects.emplace_back(std::move(objectAttributes), ObjectType::RECTANGLE);
                }
            }            

            ObjectLayerData layer(std::move(layerAttributes), std::move(objects));
            tiledMap.AddObjectLayer(std::move(layer));
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

        TilesetAttributes tilesetAttributes(ExtractString(tilesetData, "name"),
                                            ExtractUInt32(tilesetNode, "firstgid"),     // firstgid is always present in map file
                                            ExtractUInt32(tilesetData, "tilewidth"),
                                            ExtractUInt32(tilesetData, "tileheight"),
                                            columns,
                                            ExtractUInt32(tilesetData, "margin"),
                                            ExtractUInt32(tilesetData, "spacing"),
                                            ExtractUInt32(tilesetData, "tilecount"));
        
        // Spritesheet TiledSet
        if (columns > 0)
        {
            fs::path relativeImageFilePath(ExtractString(tilesetData, "image"));
            fs::path absoluteImageFilePath = fs::absolute(relativeTilesetDirectory / relativeImageFilePath);

            SpritesheetTilesetData tileset(std::move(tilesetAttributes),
                                           absoluteImageFilePath, 
                                           ExtractUInt32(tilesetData, "imageheight"),
                                           ExtractUInt32(tilesetData, "imagewidth"));
            tiledMap.AddSpritesheetTileset(std::move(tileset));
        }
        // Image Collection TiledSet
        else
        {
            std::vector<TilesetImageData> imageTiles;
            for (const json& tileNode : tilesetData["tiles"])
            {
                fs::path relativeImageFilePath(ExtractString(tileNode, "image"));
                fs::path absoluteImageFilePath = fs::absolute(relativeTilesetDirectory / relativeImageFilePath);

                TilesetImageData tile(ExtractUInt32(tileNode, "id"),
                                      absoluteImageFilePath,
                                      ExtractUInt32(tileNode, "imageheight"),
                                      ExtractUInt32(tileNode, "imagewidth"));
                imageTiles.emplace_back(tile);
            }

            ImageCollectionTilesetData tiledSet(std::move(tilesetAttributes), std::move(imageTiles));
            tiledMap.AddImageCollectionTileset(std::move(tiledSet));
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
/*static*/ uint32_t TiledMapJsonParser::ExtractBool(const json& parentNode, const std::string& key)
{
    if (!parentNode.contains(key))
    {
        throw std::invalid_argument("Invalid JSON structure: missing key - " + key);
    }
    return parentNode[key].get<bool>();
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
/*static*/ int32_t TiledMapJsonParser::ExtractInt32(const json& parentNode, const std::string& key)
{
    if (!parentNode.contains(key))
    {
        throw std::invalid_argument("Invalid JSON structure: missing key - " + key);
    }
    return parentNode[key].get<int32_t>();
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