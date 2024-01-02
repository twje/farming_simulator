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

		std::vector<std::unique_ptr<TiledMapObjectDefinition>> tiledMapObjectDefs;

		// Trees
		mTiledMap->GetObjectDefinitionsInLayer("Trees", tiledMapObjectDefs);
		for (auto& definition : tiledMapObjectDefs)
		{
			Tree* object = CreateGameObject<Tree>(std::move(definition));
			mAllSprites.Add(object);
		}
		tiledMapObjectDefs.clear();

		// Decoration
		mTiledMap->GetObjectDefinitionsInLayer("Decoration", tiledMapObjectDefs);
		for (auto& definition : tiledMapObjectDefs)
		{
			WildFlower* object = CreateGameObject<WildFlower>(std::move(definition));
			mAllSprites.Add(object);
		}
		tiledMapObjectDefs.clear();
		
		mOverlay = std::make_unique<Overlay>(assetManager, *mPlayer);
	}

	void Update(const sf::Time& timestamp) override
	{
		mTiledMap->Update(timestamp);

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
		mTiledMap->Draw(window, GetScreenViewRegion(), mAllSprites);
		
		window.setView(mHUDView);
		mOverlay->Draw(window);
	}

	virtual void OnWindowResize(const sf::Vector2u& size)
	{
		mWorldView.setSize(sf::Vector2f(size));
		mHUDView.setSize(sf::Vector2f(size));
	}

private:
	sf::FloatRect GetScreenViewRegion()
	{
		sf::Vector2f halfSize = mWorldView.getSize() / 2.0f;
		sf::Vector2f position = mWorldView.getCenter() - halfSize;
		sf::Vector2f size = halfSize * 2.0f;
		
		return sf::FloatRect(position, size);
	}

	Player* mPlayer;
	Generic* mGround;

	Group mAllSprites;
	std::unique_ptr<Overlay> mOverlay;
	sf::View mWorldView;
	sf::View mHUDView;

	TiledMap* mTiledMap;
};