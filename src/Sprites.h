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

	sf::FloatRect GetLocalBounds() const override { return mSprite.getGlobalBounds(); }
	virtual uint16_t GetDepth() const { return mDepth; }

	void Draw(sf::RenderTarget& target, const sf::RenderStates& states) const override
	{
		target.draw(mSprite, states);
	}

private:
	sf::Sprite mSprite;
	uint16_t mDepth;
};