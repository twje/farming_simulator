#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Tiled/TiledMap.h"
#include "Core/RectUtils.h"
#include "Core/Texture.h"
#include "Core/Utils.h"
#include "Core/Scene.h"
#include "Core/Timer.h"
#include "Core/Shader.h"
#include "Core/ResourceLocator.h"

#include "Settings.h"


//------------------------------------------------------------------------------
class Generic : public Sprite
{
public:
	Generic(const sf::Texture& texture, const sf::IntRect& textureRegion, const sf::Vector2f& origin, const sf::Vector2f& position, uint16_t depth=LAYERS.at("main"))
		: mSprite(texture, textureRegion),
		  mDepth(depth)
	{
		SetOrigin(origin);
		SetPosition(position);

		sf::FloatRect globalBounds = GetGlobalBounds();		
		mHitbox = InflateRect(globalBounds, -globalBounds.width * 0.2f, -globalBounds.height * 0.75);
	}

	virtual sf::FloatRect GetLocalBoundsInternal() const override 
	{ 
		return mSprite.getLocalBounds(); 
	}

	virtual sf::FloatRect GetGlobalBoundsInternal() const override 
	{ 
		return mSprite.getGlobalBounds(); 
	}

	virtual sf::FloatRect GetHitbox() const override
	{
		return mHitbox;
	}

	virtual const sf::Drawable& GetDrawable() const override { return mSprite; }	
	virtual uint16_t GetDepth() const { return mDepth; }
	const sf::Sprite& GetSprite() const { return mSprite; }

protected:
	void SetHitbox(const sf::FloatRect& hitbox)
	{
		mHitbox = hitbox;
	}

	void SetTexture(const sf::Texture& texture, const sf::IntRect& textureRect)
	{
		mSprite.setTexture(texture);
		mSprite.setTextureRect(textureRect);
	}	

private:
	sf::FloatRect mHitbox;
	sf::Sprite mSprite;
	uint16_t mDepth;
};

//------------------------------------------------------------------------------
class TiledMapObjectSprite : public Generic
{
public:
	TiledMapObjectSprite(const TiledMapObjectDefinition& definition, uint16_t depth = LAYERS.at("main"))
		: Generic(*definition.GetTexture(),
		  definition.GetTextureRegion(),
		  definition.GetOrigin(),
		  definition.GetPosition(),
		  depth)
	{ }
};

//------------------------------------------------------------------------------
class Particle : public Generic
{
public:
	Particle(const sf::Texture& texture, const sf::IntRect& textureRegion,
		     const sf::Vector2f& origin, const sf::Vector2f& position, uint16_t depth, 
		     int32_t msDuration)
		: Generic(texture, textureRegion, origin, position, depth)
		, mTimer(sf::milliseconds(msDuration))
	{ 
		AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
		SetShader(&assetManager.GetAsset<Shader>("color"));
		mTimer.Start();
	}

	virtual void Update(const sf::Time& timestamp) 
	{
		mTimer.Update(timestamp);
		if (mTimer.IsFinished())
		{
			Kill();
		}
	};

private:
	Timer mTimer;
};

//------------------------------------------------------------------------------
class WildFlower : public TiledMapObjectSprite
{
public:
	WildFlower(const TiledMapObjectDefinition& definition, uint16_t depth = LAYERS.at("main"))
		: TiledMapObjectSprite(definition, depth)
	{		
		SetOrigin(definition.GetOrigin());

		sf::FloatRect globalBounds = GetGlobalBounds();
		SetHitbox(InflateRect(globalBounds, -20.0f, -globalBounds.height * 0.9f));
	}
};

//------------------------------------------------------------------------------
class Tree : public TiledMapObjectSprite
{
public:
	Tree(const TiledMapObjectDefinition& definition, Scene& scene, Group& spriteGroup, uint16_t depth = LAYERS.at("main"))
		: TiledMapObjectSprite(definition, depth)		
		, mName(definition.GetName())
		, mScene(scene)
		, mSpriteGroup(spriteGroup)
		, mAppleGroup(scene.CreateGroup())
		, mAlive(true)
		, mHealth(5)
	{
		SetOrigin(definition.GetOrigin());
		CreateFruit();
	}

	virtual void Update(const sf::Time& timestamp)
	{
		if (mAlive)
		{
			CheckDeath();
		}
	}

	void ChopWood()
	{
		if (mAlive)
		{
			mHealth -= 1;
		}
		PickApple();		
	}

private:
	void CreateFruit()
	{			
		sf::FloatRect bounds = GetGlobalBounds();
		const sf::Vector2f position = sf::Vector2f(bounds.left, bounds.top);

		AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
		for (const sf::Vector2f& positionOffset : APPLE_POSITIONS.at(mName))
		{
			if (IsRandomNumberLessThanOrEqualTo(0, 10, 2))
			{
				const sf::Texture& texture = assetManager.GetAsset<Texture>("apple").GetRawTexture();
				const sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));				

				Generic* apple = mScene.CreateGameObject<Generic>(texture,
				    											  textureRegion, 
						                                          sf::Vector2f(), 
																  position + positionOffset,
																  LAYERS.at("fruit"));
				mSpriteGroup.Add(apple);
				mAppleGroup->Add(apple);
			}
		}
	}

	void CheckDeath()
	{
		if (mHealth <= 0)
		{
			ReplaceTreeWithStump();
			mAlive = false;					
		}
	}

	void ReplaceTreeWithStump()
	{
		AssetManager& assetManager = ResourceLocator::GetInstance().GetAssetManager();
		
		CreateSilhouetteFlash(static_cast<Generic*>(this), LAYERS.at("fruit"), 200);
		
		static const std::unordered_map<std::string, std::string> textureMap = 
		{
			{"Small", "small_stump"},
			{"Large", "large_stump"}
		};

		auto it = textureMap.find(mName);
		if (it == textureMap.end()) 
		{
			throw std::logic_error("Invalid mName value: " + mName);
		}
				
		sf::FloatRect oldBounds = GetGlobalBounds();
		sf::Texture* texture = &assetManager.GetAsset<Texture>(it->second).GetRawTexture();
		
		// Update sprite texture
		SetTexture(*texture, { sf::Vector2i(), sf::Vector2i(texture->getSize()) });
		SetOrigin({ 0.5f, 1.0f });
		SetPosition({ oldBounds.left + oldBounds.width / 2.0f, GetPosition().y });  // Tiled map origin is BL

		// Update hitbox
		sf::FloatRect newBounds = GetGlobalBounds();
		SetHitbox(InflateRect(newBounds, -10.0f, -newBounds.height * 0.6f));


		KillAllApples();
		mAlive = false;
	}

	void PickApple()
	{
		GameObject* apple = mAppleGroup->GetRandomGameObject();
		if (apple)
		{
			CreateSilhouetteFlash(static_cast<Generic*>(apple), LAYERS.at("fruit"), 200);
			apple->Kill();
		}
	}

	void KillAllApples()
	{
		for (GameObject* apple : *mAppleGroup)
		{
			if (!apple->IsMarkedForRemoval())
			{
				apple->Kill();
			}
		}
	}

	void CreateSilhouetteFlash(const Generic* source, uint16_t depth, int32_t msDuration)
	{
		const sf::FloatRect& bounds = source->GetGlobalBounds();
		const sf::Texture& texture = source->GetSprite().getTexture();
		const sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));

		Particle* particle = mScene.CreateGameObject<Particle>(texture,
															   textureRegion,
															   sf::Vector2f(),
															   sf::Vector2f(bounds.left, bounds.top),
															   depth,
															   msDuration);
		mSpriteGroup.Add(particle);
	}

	Group* mAppleGroup;
	Group& mSpriteGroup;
	Scene& mScene;
	std::string mName;
	bool mAlive;
	int32_t mHealth;
};