#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Tiled/TiledMap.h"
#include "Core/RectUtils.h"
#include "Core/Texture.h"
#include "Core/Utils.h"
#include "Core/Scene.h"

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
	Tree(AssetManager& assetManager, const TiledMapObjectDefinition& definition, Scene& scene, Group& spriteGroup, uint16_t depth = LAYERS.at("main"))
		: TiledMapObjectSprite(definition, depth)
		, mAssetManager(assetManager)		
		, mName(definition.GetName())
		, mAlive(true)
		, mHealth(5)
	{
		SetOrigin(definition.GetOrigin());
		CreateFruit(scene, spriteGroup);
	}

	virtual void Update(const sf::Time& timestamp)
	{
		if (mAlive)
		{
			CheckDeath();
		}
	}

	void InflictDemage()
	{
		if (mAlive)
		{
			mHealth -= 1;
		}

		GameObject* apple = mAppleGroup.GetRandomGameObject();
		if (apple)
		{
			apple->Kill();
		}
	}

private:
	void CreateFruit(Scene& scene, Group& spriteGroup)
	{			
		sf::FloatRect bounds = GetGlobalBounds();
		const sf::Vector2f position = sf::Vector2f(bounds.left, bounds.top);

		for (const sf::Vector2f& positionOffset : APPLE_POSITIONS.at(mName))
		{
			if (IsRandomNumberLessThanOrEqualTo(0, 10, 2))
			{
				const sf::Texture& texture = mAssetManager.GetAsset<Texture>("apple").GetRawTexture();
				const sf::IntRect textureRegion(sf::Vector2i(), sf::Vector2i(texture.getSize()));				

				Generic* apple = scene.CreateGameObject<Generic>(texture,
				    											 textureRegion, 
						                                         sf::Vector2f(), 
																 position + positionOffset,
																 LAYERS.at("fruit"));
				spriteGroup.Add(apple);
				mAppleGroup.Add(apple);
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
		static const std::unordered_map<std::string, std::string> textureMap = 
		{
			{"Small", "large_stump"},
			{"Large", "small_stump"}
		};

		auto it = textureMap.find(mName);
		if (it == textureMap.end()) 
		{
			throw std::logic_error("Invalid mName value: " + mName);
		}

		sf::FloatRect oldBounds = GetGlobalBounds();
		sf::Texture* texture = &mAssetManager.GetAsset<Texture>(it->second).GetRawTexture();		
		
		// Update sprite texture
		SetTexture(*texture, { sf::Vector2i(), sf::Vector2i(texture->getSize()) });
		SetOrigin({ 0.5f, 1.0f });
		SetPosition({ oldBounds.left + oldBounds.width / 2.0f, GetPosition().y });

		// Update hitbox
		sf::FloatRect newBounds = GetGlobalBounds();
		SetHitbox(InflateRect(newBounds, -10.0f, -newBounds.height * 0.6f));

		KillAllApples();
		mAlive = false;
	}

	void KillAllApples()
	{
		for (GameObject* apple : mAppleGroup)
		{
			if (!apple->IsMarkedForRemoval())
			{
				apple->Kill();
			}
		}
	}

	Group mAppleGroup;
	std::string mName;
	AssetManager& mAssetManager;	
	bool mAlive;
	int32_t mHealth;
};