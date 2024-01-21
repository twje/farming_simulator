#pragma once

// Includes
// --------------------------------------------------------------------------------
// Game
#include "Core/GameObject.h"
#include "Sprites.h"

// Third party
#include <SFML/Graphics.hpp>

// Core
#include "Core/Timer.h"
#include "Core/Utils.h"
#include "Core/Texture.h"

// --------------------------------------------------------------------------------
class Drop : public Generic
{
public:
	Drop(const sf::Texture& texture, const sf::IntRect& textureRegion,
	     const sf::Vector2f& position, uint16_t depth, bool isMoving)
		: Generic(texture, textureRegion, sf::Vector2f(), position, depth)
        , mTimer(sf::milliseconds(RandomInteger(400, 500)))
		, mIsMoving(isMoving)
		, mSpeed(static_cast<float>(RandomInteger(200, 250)))
		, mDirection(-2, 4)
	{         		
        mTimer.Start();
	}

	virtual void Update(const sf::Time& timestamp)
	{
		mTimer.Update(timestamp);
		
		if (mIsMoving)
		{			
			Move(mDirection * mSpeed * timestamp.asSeconds());
		}		
		
		if (mTimer.IsFinished())
		{
			Kill();
			return;
		}
	};

private:
    Timer mTimer;
	bool mIsMoving;
	float mSpeed;
	sf::Vector2f mDirection;
};

// --------------------------------------------------------------------------------
class Rain
{
public:
    Rain(Group& allSprites, Scene& scene)
        : mAllSprites(allSprites)
		, mScene(scene)
    { }

    void Update(ViewRegion viewRegion)
    {
		CreateFloor(viewRegion.GetScreenViewRegion());
		CreateDrop(viewRegion.GetScreenViewRegion());
    }

	void CreateFloor(const sf::FloatRect& screenViewRegion)
	{
		static std::vector<std::string> textureIds = { "floor_0", "floor_1", "floor_2" };

		CreateSprite(textureIds.at(RandomInteger(0, 2)),
 					 GetRandomScreenPosition(screenViewRegion),
					 6,
					 false);
	}

	void CreateDrop(const sf::FloatRect& screenViewRegion)
	{
		static std::vector<std::string> textureIds = { "drop_0", "drop_1", "drop_2" };

		CreateSprite(textureIds.at(RandomInteger(0, 2)), 
					 GetRandomScreenPosition(screenViewRegion), 
			         6, 
			         true);
	}

private:
	void CreateSprite(const std::string& textureId, const sf::Vector2f& position, uint16_t depth, bool isMoving) 
	{
		sf::Texture& texture = GetTextureFromId(textureId);
		sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));

		GameObject* sprite = mScene.CreateGameObject<Drop>(texture, textureRegion, position, depth, isMoving);
		mAllSprites.Add(sprite);
	}

	sf::Vector2f GetRandomScreenPosition(const sf::FloatRect& screenViewRegion)
	{
		int32_t xOffset = RandomInteger(0, static_cast<int32_t>(screenViewRegion.width));
		int32_t yOffset = RandomInteger(0, static_cast<int32_t>(screenViewRegion.height));
		return screenViewRegion.getPosition() + sf::Vector2f(xOffset, yOffset);
	}

	sf::Texture& GetTextureFromId(const std::string& textureId) 
	{
		ResourceLocator& locator = ResourceLocator::GetInstance();
		AssetManager& assetManager = locator.GetAssetManager();
		Texture& textureContainer = assetManager.GetAsset<Texture>(textureId);
		return textureContainer.GetRawTexture();
	}

	Group& mAllSprites;
    Scene& mScene;
};