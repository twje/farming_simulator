#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Tiled/TiledMap.h"

#include "Settings.h"

//------------------------------------------------------------------------------
class Generic : public Sprite
{
public:
	Generic(const sf::Texture& texture, const sf::Vector2f& position, uint16_t depth=LAYERS.at("main"))
		: mSprite(texture),
		  mDepth(depth)
	{
		SetPosition(position);
	}

	virtual sf::FloatRect GetLocalBoundsInternal() const override { return mSprite.getLocalBounds(); }
	virtual sf::FloatRect GetGlobalBoundsInternal() const override { return mSprite.getGlobalBounds(); }
	virtual const sf::Drawable& GetDrawable() const override { return mSprite; }
	
	virtual uint16_t GetDepth() const { return mDepth; }

private:
	sf::Sprite mSprite;
	uint16_t mDepth;
};

//------------------------------------------------------------------------------
class WildFlower : public Sprite
{
public:
	WildFlower(std::unique_ptr<TiledMapObjectDefinition> definition, uint16_t depth = LAYERS.at("main"))
		: mDefinition(std::move(definition))
		, mSprite(mDefinition->GetTexture()),
		  mDepth(depth)
	{
		sf::IntRect textureRect = mSprite.getTextureRect();
		mSprite.setOrigin({ 0.0f, static_cast<float>(textureRect.height) }); // tiled map object origin is BL

		tson::Vector2i position = mDefinition->GetData().getPosition();
		mSprite.setPosition({ static_cast<float>(position.x), static_cast<float>(position.y) });
	}

	virtual sf::FloatRect GetLocalBoundsInternal() const override { return mSprite.getLocalBounds(); }
	virtual sf::FloatRect GetGlobalBoundsInternal() const override { return mSprite.getGlobalBounds(); }
	virtual const sf::Drawable& GetDrawable() const override { return mSprite; }

	virtual uint16_t GetDepth() const { return mDepth; }

private:
	std::unique_ptr<TiledMapObjectDefinition>& mDefinition;
	sf::Sprite mSprite;
	uint16_t mDepth;
};

//------------------------------------------------------------------------------
class Tree : public Sprite
{
public:
	Tree(std::unique_ptr<TiledMapObjectDefinition> definition, uint16_t depth = LAYERS.at("main"))
		: mDefinition(std::move(definition))
		, mSprite(mDefinition->GetTexture()),
		  mDepth(depth)
	{
		sf::IntRect textureRect = mSprite.getTextureRect();
		mSprite.setOrigin({ 0.0f, static_cast<float>(textureRect.height) }); // tiled map object origin is BL
		
		tson::Vector2i position = mDefinition->GetData().getPosition();
		mSprite.setPosition({ static_cast<float>(position.x), static_cast<float>(position.y) });
	}

	virtual sf::FloatRect GetLocalBoundsInternal() const override { return mSprite.getLocalBounds(); }
	virtual sf::FloatRect GetGlobalBoundsInternal() const override { return mSprite.getGlobalBounds(); }
	virtual const sf::Drawable& GetDrawable() const override { return mSprite; }

	virtual uint16_t GetDepth() const { return mDepth; }

private:
	std::unique_ptr<TiledMapObjectDefinition>& mDefinition;
	sf::Sprite mSprite;
	uint16_t mDepth;
};