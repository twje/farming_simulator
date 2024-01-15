#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Settings.h"

// Core
#include "Core/ResourceLocator.h"
#include "Sprites.h"

#include "Core/Scene.h"
#include "Core/Tiled/TiledMap.h"

//------------------------------------------------------------------------------
struct SoilCell
{
    bool mFarmable{ false };
    bool mIsHit{ false };
    sf::FloatRect mBounds;
    sf::Vector2i mTileIndex;
};

//------------------------------------------------------------------------------
class SoilLayer
{
public:
    SoilLayer(Group& allSprites, Scene& scene)
        : mAllSprites(allSprites)
        , mScene(scene)
        , mSoilSprites(*scene.CreateGroup())
    { 
        AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
        mMap = &assetManager.GetAsset<TiledMap>("main");                        
        mGrid.resize(mMap->GetTileCount());        
        
        tson::Layer* farmableLayer = mMap->GetLayerByName("Farmable");
        assert(farmableLayer);                
        for (const auto& pair : farmableLayer->getTileObjects()) // Only returns non-empty tiles
        {            
            tson::Vector2f pixelPos = pair.second.getPosition();
            tson::Vector2i tilePos = pair.second.getPositionInTileUnits();
            SoilCell& soilTile = mGrid.at(TileIndex(tilePos.x, tilePos.y));
            
            soilTile.mBounds = sf::FloatRect(sf::Vector2f(pixelPos.x, pixelPos.y), mMap->GetTileSize());
            soilTile.mTileIndex = sf::Vector2i(tilePos.x, tilePos.y);
            soilTile.mFarmable = true;            
        }
    }

    void foo(const sf::Vector2f point)
    {        
        for (SoilCell& soilTile : mGrid)
        {
            if (soilTile.mBounds.contains(point))
            {
                soilTile.mIsHit = true;
                CreateSoilTiles();
            }
        }        
    }

private:
    void CreateSoilTiles()
    {
        for (GameObject* gameObject : mSoilSprites)
        {
            gameObject->Kill();
        }

        for (const SoilCell& tile : mGrid)
        {
            if (tile.mIsHit)
            {
                bool t = mGrid.at(TileIndex(tile.mTileIndex.x, tile.mTileIndex.y - 1)).mIsHit;
                bool b = mGrid.at(TileIndex(tile.mTileIndex.x, tile.mTileIndex.y + 1)).mIsHit;
                bool r = mGrid.at(TileIndex(tile.mTileIndex.x + 1, tile.mTileIndex.y)).mIsHit;
                bool l = mGrid.at(TileIndex(tile.mTileIndex.x - 1, tile.mTileIndex.y)).mIsHit;

                std::string tileType = "o";

                // all sides
                if (t && b && r && l) { tileType = "x"; }

                // horizontal tiles only
                if (l && !(t || r || b)) { tileType = "r"; }
                if (r && !(t || l || b)) { tileType = "l"; }
                if (r && l && !(t || b)) { tileType = "lr"; }

                // vertical tiles only
                if (t && !(r || l || b)) { tileType = "b"; }
                if (b && !(r || l || t)) { tileType = "t"; }
                if (b && t && !(r || l)) { tileType = "tb"; }

                // corners
                if (l && b && !(t || r)) { tileType = "tr"; }
                if (r && b && !(t || l)) { tileType = "tl"; }
                if (l && t && !(b || r)) { tileType = "br"; }
                if (r && t && !(b || l)) { tileType = "bl"; }

                // T shapes
                if ((t && b && r) && !l) { tileType = "tbr"; }
                if ((t && b && l) && !r) { tileType = "tbl"; }
                if ((l && r && t) && !b) { tileType = "lrb"; }
                if ((l && r && b) && !t) { tileType = "lrt"; }

                AddSoilTile(tileType, tile.mTileIndex);
            }
        }
    }

    void AddSoilTile(const std::string& tileType, sf::Vector2i tileIndex)
    {
        AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
        sf::Texture& texture = assetManager.GetAsset<Texture>(tileType).GetRawTexture();
        sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));
        
        float posX = tileIndex.x * mMap->GetTileSize().x;
        float posY = tileIndex.y * mMap->GetTileSize().y;

        auto sprite = mScene.CreateGameObject<Generic>(texture,
                                                       textureRegion,
                                                       sf::Vector2f(),
                                                       sf::Vector2f(posX, posY));
        mSoilSprites.Add(sprite);
        mAllSprites.Add(sprite);
    }

    size_t TileIndex(size_t x, size_t y)
    {
        return x + y * mMap->GetTileCount2Dim().x;
    }

    std::vector<SoilCell> mGrid;
    Group& mAllSprites;
    Scene& mScene;
    Group& mSoilSprites;
    TiledMap* mMap;
};