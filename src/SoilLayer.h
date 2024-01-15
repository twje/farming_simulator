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

        AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
        sf::Texture& texture = assetManager.GetAsset<Texture>("o").GetRawTexture();                
        sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));

        for (const SoilCell& soilTile : mGrid)
        {
            if (soilTile.mIsHit)
            {
                float posX = soilTile.mTileIndex.x * mMap->GetTileSize().x;
                float posY = soilTile.mTileIndex.y * mMap->GetTileSize().y;

                auto sprite =  mScene.CreateGameObject<Generic>(texture,
                                                                textureRegion,
                                                                sf::Vector2f(),
                                                                sf::Vector2f(posX, posY));
                mSoilSprites.Add(sprite);
                mAllSprites.Add(sprite);
            }
        }
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