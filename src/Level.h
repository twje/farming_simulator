#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Player.h"

// Core
#include "Core/Scene.h"
#include "Core/Group.h"
#include "Core/AssetManager.h"
#include "Core/Tiled/TiledMap.h"

#include <iostream>
#include "Overlay.h"
#include "Sprites.h"

//------------------------------------------------------------------------------
class Level : public Scene
{
public:
	void Create() override
	{
		AssetManager& assetManager = GetResourceLocator().GetAssetManager();



		const ApplicationConfig& config = GetResourceLocator().GetApplicationConfig();
		sf::Vector2f windowSize = sf::Vector2f(config.GetWindowSize());

		mWorldView.setSize(windowSize);
		mWorldView.setCenter(windowSize * 0.5f);

		mHUDView.setSize(windowSize);
		mHUDView.setCenter(windowSize * 0.5f);

		mPlayer = CreateGameObject<Player>(assetManager, sf::Vector2f(0, 0));
		mAllSprites.Add(mPlayer);

		mTiledMap = &assetManager.GetAsset<TiledMap>("main");

		// Trees
		for (auto& definition : mTiledMap->GetObjectDefinitions("Trees"))
		{
			Tree* object = CreateGameObject<Tree>(std::move(definition));
			mAllSprites.Add(object);
		}

		// Decoration
		for (auto& definition : mTiledMap->GetObjectDefinitions("Decoration"))
		{
			WildFlower* object = CreateGameObject<WildFlower>(std::move(definition));
			mAllSprites.Add(object);
		}

		mOverlay = std::make_unique<Overlay>(assetManager, *mPlayer);
	}

	void Update(const sf::Time& timestamp) override
	{
		mTiledMap->Update(timestamp);

		for (GameObject* gameObject : mAllSprites)
		{
			if (gameObject->IsMarkedForRemoval()) 
			{ 
				continue; 
			}

			gameObject->Update(timestamp);
		}

		mWorldView.setCenter(mPlayer->GetCenter());
	}

	sf::FloatRect GetViewRegion(const sf::View& view)
	{
		sf::Vector2f center = view.getCenter();
		sf::Vector2f size = view.getSize();

		sf::Vector2f topLeft((center.x - size.x / 2), (center.y - size.y / 2));
		sf::Vector2f bottomRight((center.x + size.x / 2), (center.y + size.y / 2));

		return sf::FloatRect(topLeft, bottomRight - topLeft);
	}

	virtual void Draw(sf::RenderWindow& window)
	{
		window.setView(mWorldView);

		mAllSprites.Sort(SpriteCompareFunc);

		const ViewRegion viewRegion = GetViewRegion();
		for (size_t layerIndex = 0; layerIndex < mTiledMap->LayerCount(); layerIndex++)
		{
			if (mTiledMap->GetLayerType(layerIndex) == LayerType::TileLayer)
			{
				mTiledMap->DrawLayer(layerIndex, window, viewRegion);
			}

			for (GameObject* gameObject : mAllSprites)
			{
				if (gameObject->IsMarkedForRemoval())
				{
					continue;
				}

				if (gameObject->GetDepth() == layerIndex)
				{
					window.draw(*gameObject);
				}
			}
		}
				
		window.setView(mHUDView);
		mOverlay->Draw(window);
	}

	virtual void OnWindowResize(const sf::Vector2u& size)
	{
		mWorldView.setSize(sf::Vector2f(size));
		mHUDView.setSize(sf::Vector2f(size));
	}

private:
	static bool SpriteCompareFunc(const GameObject* object1, const GameObject* object2)
	{		
		float y1 = static_cast<const Sprite*>(object1)->GetCenter().y;
		float y2 = static_cast<const Sprite*>(object2)->GetCenter().y;

		return y1 < y2;
	}

	ViewRegion GetViewRegion()
	{
		sf::Vector2f halfSize = mWorldView.getSize() / 2.0f;
		sf::Vector2f position = mWorldView.getCenter() - halfSize;
		sf::Vector2f size = halfSize * 2.0f;
		
		return { mTiledMap->GetTileSize(), mTiledMap->GetMapSize(), { position, size } };
	}

	Player* mPlayer;
	Generic* mGround;

	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
	sf::View mWorldView;
	sf::View mHUDView;

	TiledMap* mTiledMap;
};