#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Core/GameObject.h"
#include "Core/ResourceLocator.h"
#include "Core/Animation/Animation.h"
#include "Core/Support.h"
#include "Core/Group.h"
#include "Core/ItemPicker.h"
#include "Core/AssetManager.h"
#include "Core/Animation/AnimationPlayer.h"
#include "Core/RectUtils.h"

#include "Settings.h"
#include "Sprites.h"
#include "Tree.h"
#include "SoilLayer.h"

// --------------------------------------------------------------------------------
enum class TimerId
{
	TOOL_USE = 0,
	TOOL_SWITCH = 1,
	SEED_USE = 2,
	SEED_SWITCH = 3
};

// --------------------------------------------------------------------------------
class IPlayerObserver
{
public:
	virtual void ToolChanged(const std::string& tool) { }
	virtual void SeedChanged(const std::string& seed) { }
	virtual void WentToSleep() { }
};

// --------------------------------------------------------------------------------
class PlayerSubject
{
public:
	void Subscribe(IPlayerObserver* observer) { mObservers.emplace_back(observer); }

	void NotifyToolChanged(std::string tool)
	{
		for (auto& observer : mObservers)
		{
			observer->ToolChanged(tool);
		}
	}

	void NotifySeedChanged(std::string seed)
	{
		for (auto& observer : mObservers)
		{
			observer->SeedChanged(seed);
		}
	}

	void NotifyWentToSleep()
	{
		for (auto& observer : mObservers)
		{
			observer->WentToSleep();
		}
	}

private:
	std::vector<IPlayerObserver*> mObservers;
};

// --------------------------------------------------------------------------------
class Player : public Sprite, public PlayerSubject
{
public:
	Player(AssetManager& assetManager, const sf::Vector2f& position, Group& collisionSprites, 
		   Group& interactionSprites, Group& treeSprites, SoilLayer& soilLayer, uint16_t depth)
		: mCollisionSprites(collisionSprites),
		  mInteractionSprites(interactionSprites),
		  mTreeSprites(treeSprites),
		  mSoilLayer(soilLayer),
		  mAnimationPlayer(assetManager.GetAsset<Animation>("character")),
		  mSpeed(300),
		  mStatus("down_idle"),
		  mToolPicker({ "hoe", "axe", "water"}),
		  mSeedPicker({ "corn", "tomato" }),
		  mDepth(depth),
		  mIsAsleep(false)
	{		
		mAnimationPlayer.SetAnimationSequence(mStatus);
		SetPosition(position);
		SetOrigin(sf::Vector2f(0.5f, 0.5f));

		mTimers.emplace(TimerId::TOOL_USE, Timer(sf::milliseconds(350), std::bind(&Player::UseTool, this)));
		mTimers.emplace(TimerId::TOOL_SWITCH, Timer(sf::milliseconds(200)));
		mTimers.emplace(TimerId::SEED_USE, Timer(sf::milliseconds(350), std::bind(&Player::UseSeed, this)));
		mTimers.emplace(TimerId::SEED_SWITCH, Timer(sf::milliseconds(200)));
			
		mHitbox = InflateRect(GetGlobalBounds(), -127, -70);

		mInventory = { 
			{ "water", 0}, 
			{ "apple", 0}, 
			{ "corn", 0}, 
			{ "tomoato", 0}, 
			{ "wood", 0},
		};
	}

	virtual sf::FloatRect GetLocalBoundsInternal() const override
	{
		return mAnimationPlayer.GetSprite().getLocalBounds();
	}

	virtual sf::FloatRect GetGlobalBoundsInternal() const override
	{
		return mAnimationPlayer.GetSprite().getGlobalBounds();
	}

	virtual const sf::Drawable& GetDrawable() const override
	{
		return mAnimationPlayer.GetSprite();
	}

	uint16_t GetDepth() const override { return mDepth; }
	std::string GetActiveTool() const { return mToolPicker.GetItem(); }
	std::string GetActiveSeed() const { return mSeedPicker.GetItem(); }

	void AddItemToInventory(const std::string& item)
	{
		mInventory.at(item)++;
	}

	void UseTool()
    {		
		if (mToolPicker.GetItem() == "hoe")
		{
			mSoilLayer.HoeSoil(mTargetPosition);
		}
		if (mToolPicker.GetItem() == "water")
		{
			mSoilLayer.WaterSoil(mTargetPosition);
		}
		else if (mToolPicker.GetItem() == "axe")
		{
			for (GameObject* gameObject : mTreeSprites)
			{
				Tree* tree = static_cast<Tree*>(gameObject);
				if (tree->GetGlobalBounds().contains(mTargetPosition))
				{
					tree->ChopWood();
					break;
				}
			}
		}
	}

	void UseSeed()
	{
		std::cout << mSeedPicker.GetItem() << std::endl;
	}

	void Input()
	{
		if (!mTimers[TimerId::TOOL_USE].IsActive() && !mIsAsleep)
		{
			// directions
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
			{
				mDirection.y = -1;
				mStatus = "up";
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
			{
				mDirection.y = 1;
				mStatus = "down";
			}
			else
			{
				mDirection.y = 0;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
			{
				mDirection.x = 1;
				mStatus = "right";
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
			{
				mDirection.x = -1;
				mStatus = "left";
			}
			else
			{
				mDirection.x = 0;
			}

			// tool use
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
			{
				mTimers[TimerId::TOOL_USE].Start();
				mDirection = sf::Vector2f();
			}

			// change tool
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) && !mTimers[TimerId::TOOL_SWITCH].IsActive())
			{
				mTimers[TimerId::TOOL_SWITCH].Start();
				mToolPicker.Next();
				NotifyToolChanged(mToolPicker.GetItem());
			}

			// seed use
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
			{
				mTimers[TimerId::SEED_USE].Start();
				mDirection = sf::Vector2f();
			}

			// change seed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E) && !mTimers[TimerId::SEED_SWITCH].IsActive())
			{
				mTimers[TimerId::SEED_SWITCH].Start();
				mSeedPicker.Next();
				NotifySeedChanged(mSeedPicker.GetItem());
			}

			// Sleep
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
			{								
				for (GameObject* gameObject : mInteractionSprites)
				{
					auto interaction = static_cast<Interaction*>(gameObject);										
					if (interaction->GetGlobalBounds().findIntersection(GetHitbox()))
					{
						return;
					}

					if (interaction->GetName() == "Trader")
					{
						// TODO: implement
					}
					else
					{
						mIsAsleep = true;
						mStatus = "left_idle";
						NotifyWentToSleep();						
						break;
					}
				}
			}
		}
	}

	void WakeUp()
	{
		mIsAsleep = false;
	}

	void GetStatus()
	{
		// idle
		if (mDirection.lengthSq() == 0)
		{
			mStatus = SplitAndGetElement(mStatus, '_', 0) + "_idle";
		}

		// tool use
		if (mTimers[TimerId::TOOL_USE].IsActive())
		{
			mStatus = SplitAndGetElement(mStatus, '_', 0) + "_" + mToolPicker.GetItem();
		}
	}

	void UpdateTimers(const sf::Time& timestamp)
	{
		for (auto& pair : mTimers) {
			pair.second.Update(timestamp);
			if (pair.second.IsFinished())
			{
				pair.second.Reset();
			}
		}
	}

	void HortCollision()
	{
		for (GameObject* gameObject : mCollisionSprites)
		{
			const sf::FloatRect& targetHitbox = static_cast<Sprite*>(gameObject)->GetHitbox();
			if (mHitbox.findIntersection(targetHitbox))
			{
				if (mDirection.x > 0)
				{
					mHitbox.left = targetHitbox.left - mHitbox.width;
				}
				else if (mDirection.x < 0)
				{
					mHitbox.left = targetHitbox.left + targetHitbox.width;
				}
			}
		}
	}

	void VertCollision()
	{
		for (GameObject* gameObject : mCollisionSprites)
		{
			const sf::FloatRect& targetHitbox = static_cast<Sprite*>(gameObject)->GetHitbox();
			if (mHitbox.findIntersection(targetHitbox))
			{
				if (mDirection.y > 0)
				{
					mHitbox.top = targetHitbox.top - mHitbox.height;
				}
				else if (mDirection.y < 0)
				{
					mHitbox.top = targetHitbox.top + targetHitbox.height;
				}
			}
		}
	}

	void Move(const sf::Time& timestamp)
	{
		if (mDirection.lengthSq() != 0)
		{
			mDirection = mDirection.normalized();
		}

		sf::Vector2f positionDelta = mDirection * mSpeed * timestamp.asSeconds();
		
		mHitbox.left += positionDelta.x;
		HortCollision();
		
		mHitbox.top += positionDelta.y;
		VertCollision();

		sf::Vector2f center = GetRectCenter(mHitbox);
		SetPosition(sf::Vector2f(static_cast<int32_t>(center.x), static_cast<int32_t>(center.y)));
	}

	void Animate(const sf::Time& timestamp)
	{
		mAnimationPlayer.SetAnimationSequence(mStatus);
		mAnimationPlayer.Upate(timestamp);
	}

	void Update(const sf::Time& timestamp) override
	{
		Input();
		GetStatus();
		UpdateTimers(timestamp);
		Move(timestamp);
		UpdateTargetPosition();
		Animate(timestamp);
	};

	const sf::Vector2f& GetTargetPosition() const { return mTargetPosition; }

private:
	void UpdateTargetPosition()
	{
		const sf::FloatRect globalBounds = GetGlobalBounds();
		mTargetPosition = globalBounds.getCenter() + PLAYER_TOOL_OFFSET.at(SplitAndGetElement(mStatus, '_', 0));
	}

	std::map<std::string, int32_t> mInventory;
	sf::Vector2f mTargetPosition;
	sf::FloatRect mHitbox;
	sf::Vector2f mDirection;
	float mSpeed;
	std::string mStatus;
	AnimationPlayer mAnimationPlayer;
	std::unordered_map<TimerId, Timer> mTimers;
	std::string mSelectedTool;
	ItemPicker<std::string> mToolPicker;
	ItemPicker<std::string> mSeedPicker;
	Group& mCollisionSprites;
	Group& mInteractionSprites;
	Group& mTreeSprites;
	SoilLayer& mSoilLayer;
	uint16_t mDepth;
	bool mIsAsleep;
};