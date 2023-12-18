#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include <Core/Json/NlohmannJson.h>

// System
#include <filesystem>

using json = nlohmann::json;

namespace fs = std::filesystem;

namespace test
{

// Forward declarations
//------------------------------------------------------------------------------
class Tileset;

//------------------------------------------------------------------------------
enum class LayerType : uint8_t
{
	Undefined = 0,
	TileLayer = 1,
	ObjectGroup = 2,
	ImageLayer = 3,		// not supported
	Group = 4			// not supported
};

//------------------------------------------------------------------------------
enum class ParseStatus : uint8_t
{
	OK = 0, //OK unless otherwise stated
	FileNotFound = 1,
	ParseError = 2,
	MissingData = 3,
	DecompressionError = 4
};

//------------------------------------------------------------------------------
class Layer
{
public:
	Layer(IJson& json)
	{
		Parse(json);
	}

	bool Parse(IJson& json)
	{
		bool allFound = true;

		// Mandatory
		allFound &= json.TryGet("type", mTypeStr);
		allFound &= json.TryGet("name", mName);

		// Optional

		SetTypeByString();

		return allFound;
	}

	const std::string& GetName() const { return mName; }

private:
	void SetTypeByString()
	{
		if (mTypeStr == "tilelayer")
		{
			mType = LayerType::TileLayer;
		}
		else if (mTypeStr == "objectgroup")
		{
			mType = LayerType::ObjectGroup;
		}
		else if (mTypeStr == "imagelayer")
		{
			mType = LayerType::ImageLayer;
		}
		else if (mTypeStr == "group")
		{
			mType = LayerType::Group;
		}
		else
		{
			mType = LayerType::Undefined;
		}
	}
	
	std::string mTypeStr;
	LayerType mType = LayerType::Undefined;
	std::string mName;
};

//------------------------------------------------------------------------------
class Tile
{
public:
	Tile(uint32_t id, Tileset* tileset);
	Tile(IJson& json, Tileset* tileset);

	bool Parse(IJson& json, Tileset* tileset)
	{
		mTileset = tileset;
		bool allFound = ParseId(json);
		return allFound;
	}

	uint32_t GetId() const { return mId; }
	uint32_t GetGid() const { return mGid; }

private:
	bool ParseId(IJson& json);
	void ComputeGid();

	uint32_t mId = 0;
	uint32_t mGid = 0;
	Tileset* mTileset = nullptr;
};

//------------------------------------------------------------------------------
class Tileset
{
public:
	bool Parse(IJson& json)
	{
		bool allFound = true;
		
		allFound &= json.TryGet("firstgid", mFirstGid);
		if (json.Count("source") > 0)
		{
			if (!allFound)
			{
				return false;
			}
			std::string sourceStr = json["source"].Get<std::string>();
		
			mSource = fs::path(sourceStr);
			mPath = json.GetDirectory() / mSource;

			if (!json.Parse(mPath)) // clear cache // key: value -> wrapper/value
				return false;
		}

		// Mandatory
		allFound &= json.TryGet("type", mType);
		allFound &= json.TryGet("name", mName);
		allFound &= json.TryGet("tilecount", mTileCount);
		allFound &= json.TryGet("columns", mColumns);
		allFound &= json.TryGet("margin", mMargin);
		allFound &= json.TryGet("spacing", mSpacing);
		allFound &= json.TryGet("tilewidth", mTileWith);
		allFound &= json.TryGet("tileheight", mTileHeight);

		if (json.Count("tiles") > 0 && json["tiles"].IsArray())
		{
			auto& tiles = json.Array("tiles");
			std::for_each(tiles.begin(), tiles.end(), [&](std::unique_ptr<IJson>& item) { 
				mTiles.emplace_back(*item, this);
			});
		}

		GenerateMissingTiles();

		return allFound;
	}

	Tile* GetTile(uint32_t id)
	{
		auto result = std::find_if(mTiles.begin(), mTiles.end(), [&](const test::Tile& item) { 
			return item.GetId() == id; 
		});

		if (result == mTiles.end())
		{
			return nullptr;
		}
		return &result.operator*();
	}

	const std::string& GetType() const { return mType; }
	const std::string& GetName() const { return mName; }
	const uint32_t GetFirstGid() const { return mFirstGid; }
	const uint32_t GetTileCount() const { return mTileCount; }
	const uint32_t GetColumns() const { return mColumns; }
	const uint32_t GetMargin() const { return mMargin; }
	const uint32_t GetSpacing() const { return mSpacing; }
	const uint32_t GetTileWidth() const { return mTileWith; }
	const uint32_t GetTileHeight() const { return mTileHeight; }

private:
	void GenerateMissingTiles()
	{
		std::vector<uint32_t> tileIds;
		for (auto& tile : mTiles)
		{
			tileIds.push_back(tile.GetId());
		}

		for (uint32_t id = 0; id < mTileCount; ++id)
		{
			if (std::count(tileIds.begin(), tileIds.end(), id) == 0)
			{
				mTiles.emplace_back(id, this);
			}
		}
	}

	std::string mType;
	std::string mName;
	uint32_t mFirstGid;
	uint32_t mTileCount;
	uint32_t mColumns;
	uint32_t mMargin;
	uint32_t mSpacing;
	uint32_t mTileWith;
	uint32_t mTileHeight;
	std::vector<test::Tile> mTiles;
	fs::path mSource;
	fs::path mPath;
};

//------------------------------------------------------------------------------
class TiledMap
{
public:
	TiledMap() = default;
	TiledMap(ParseStatus status, const std::string& statusMessage)
		: mStatus(status)
		, mStatusMessage(statusMessage)
	{ }

	ParseStatus GetParseStatus() const { return mStatus; }
	const std::string& GetParseStatusMessage() const { return mStatusMessage; }	

	std::vector<test::Layer>& GetLayers() { return mLayers; }
	std::vector<test::Tileset>& GetTilesets() { return mTilesets; }

	bool Parse(IJson& json)
	{
		bool allFound = true;
		if (json.Count("layers") > 0 && json["layers"].IsArray())
		{
			auto& layers = json.Array("layers");
			std::for_each(layers.begin(), layers.end(), [&](std::unique_ptr<IJson>& item) { 
				mLayers.emplace_back(*item); 
			});
		}

		allFound &= CreateTilesetData(json);

		return allFound;
	}

private:
	/*!
     * Tileset data must be created in two steps to prevent malformed tson::Tileset pointers inside tson::Tile
     */
	bool CreateTilesetData(IJson& json)
	{
		bool parsedCorrectly = true;
		if (json.Count("tilesets") > 0 && json["tilesets"].IsArray())
		{
			// Step 1. Create tileset objects
			auto& tilesets = json.Array("tilesets");
			std::for_each(tilesets.begin(), tilesets.end(), [&](std::unique_ptr<IJson>&)
			{
				mTilesets.emplace_back();
			});

			// Step 2. Parse tileset objects
			size_t index = 0;
			std::for_each(tilesets.begin(), tilesets.end(), [&](std::unique_ptr<IJson>& item)
			{
				item->SetDirectory(json.GetDirectory());
				if(!mTilesets[index].Parse(*item))
				{
					parsedCorrectly = false;
				}
				index += 1;
			});
		}

		return parsedCorrectly;
	}

	ParseStatus mStatus{ ParseStatus::OK };
	std::string mStatusMessage{ "OK" };
	
	std::vector<test::Layer> mLayers;
	std::vector<Tileset> mTilesets;
};

//------------------------------------------------------------------------------
class TiledMapParser
{
public:
	explicit TiledMapParser(std::unique_ptr<test::IJson> json = std::make_unique<test::NlohmannJson>())
		: mJson(std::move(json))
	{ }

	std::unique_ptr<TiledMap> Parse(fs::path filepath)
	{
		if (mJson->Parse(filepath))
		{
			return ParseJson();
		}

		std::string msg = "File not found: ";
		msg += std::string(filepath.generic_string());
		return std::make_unique<test::TiledMap>(test::ParseStatus::FileNotFound, msg);
	}

	std::unique_ptr<test::TiledMap> ParseJson()
	{
		std::unique_ptr<test::TiledMap> map = std::make_unique<test::TiledMap>();

		if (map->Parse(*mJson))
		{
			return map;
		}			

		return std::make_unique<test::TiledMap>(test::ParseStatus::MissingData, "Missing map data...");
	}

private:
	std::unique_ptr<test::IJson> mJson;
};
}