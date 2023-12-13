#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Player.h"

// Core
#include "Core/Scene.h"
#include "Core/Group.h"
#include "Core/AssetManager.h"
#include "Core/Tiled/TiledMapRenderer.h"
#include "Core/Tiled/TiledMap.h"

#include <iostream>
#include "Overlay.h"
#include "Sprites.h"

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
		//mGround = CreateGameObject<Generic>(assetManager.GetAsset<Texture>("ground").GetRawTexture(), sf::Vector2f(0, 0), LAYERS.at("ground"));

		mAllSprites.Add(mPlayer);
		//mAllSprites.Add(mGround);

		mOverlay = std::make_unique<Overlay>(assetManager, *mPlayer);

		TiledMapAsset& tiledMap = assetManager.GetAsset<TiledMapAsset>("main");
		mTiledMapRenderer = new TiledMapRenderer(tiledMap);
		
		TiledMap foo(tiledMap);
	}

	void Update(const sf::Time& timestamp) override
	{
		for (GameObject* gameObject : mAllSprites)
		{
			if (!gameObject->IsMarkedForRemoval()) { continue; }

			gameObject->Update(timestamp);
		}

		mWorldView.setCenter(mPlayer->GetCenter());
	}

	sf::Vector2f Lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) 
	{
		return sf::Vector2f(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t));
	}	

	float Lerp(float a, float b, float t)
	{
		return (a * (1.0f - t) + b * t);
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
		mTiledMapRenderer->Draw(window, GetViewRegion(mWorldView));

		for (size_t z = 0; z < LAYERS.size(); z++)
		{
			for (GameObject* gameObject : mAllSprites)
			{
				if (!gameObject->IsMarkedForRemoval()) { continue; }

				if (z == gameObject->GetDepth())
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
	Player* mPlayer;
	Generic* mGround;	
	TiledMapRenderer* mTiledMapRenderer;

	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
	sf::View mWorldView;
	sf::View mHUDView;
};