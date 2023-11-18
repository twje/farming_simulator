#pragma once

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"

class Generic : public Sprite
{
public:
	Generic(const sf::Texture& texture, const sf::Vector2f& position, uint16_t depth)
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