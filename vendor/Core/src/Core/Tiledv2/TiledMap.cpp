#include "Core/Tiledv2/TiledMap.h"


namespace test
{
	Tile::Tile(uint32_t id, Tileset* tileset)
		: mId{ id }
		, mTileset(tileset)
	{ 
		ComputeGid();
	}

	Tile::Tile(IJson& json, Tileset* tileset)
	{
		Parse(json, tileset);
	}

	bool Tile::ParseId(IJson& json)
	{
		if (json.TryGet("id", mId));
		{
			ComputeGid();
		}
		return false;
	}

	void Tile::ComputeGid()
	{
		assert(mTileset);
		mGid = mTileset->GetFirstGid() + mId;
	}
}

