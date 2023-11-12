#pragma once

#include <iostream>

#include "Core/Scene.h"
#include "Core/Group.h"
#include "Core/AssetManager.h"

#include "Player.h"
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

		mPlayer = CreateGameObject<Player>(assetManager, sf::Vector2f(640, 360));
		mGround = CreateGameObject<Generic>(assetManager.GetAsset<Texture>("ground").GetRawTexture(), sf::Vector2f(0, 0), LAYERS.at("ground"));

		mAllSprites.Add(mPlayer);
		mAllSprites.Add(mGround);

		mOverlay = std::make_unique<Overlay>(assetManager, *mPlayer);
	}

	void Update(const sf::Time& timestamp) override
	{
		mWorldView.setCenter(mPlayer->GetCenter());

		for (GameObject* gameObject : mAllSprites)
		{
			if (!gameObject->IsMarkedForRemoval()) { continue; }

			gameObject->Update(timestamp);
		}
	}

	virtual void Draw(sf::RenderWindow& window)
	{
		window.setView(mWorldView);
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

	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
	sf::View mWorldView;
	sf::View mHUDView;
};