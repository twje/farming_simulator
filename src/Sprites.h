#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Tiled/TiledMap.h"
#include "Core/RectUtils.h"

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
	Tree(const TiledMapObjectDefinition& definition, uint16_t depth = LAYERS.at("main"))
		: TiledMapObjectSprite(definition, depth)
		, mName(definition.GetName())
	{
		SetOrigin(definition.GetOrigin());
	}

private:
	std::string mName;
};