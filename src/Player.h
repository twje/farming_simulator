#pragma once

#include <iostream>

#include "Core/GameObject.h"
#include "Core/ResourceLocator.h"
#include "Core/Animation.h"

class Player : public Sprite
{
public:
	Player(ResourceLocator& locator, const sf::Vector2f& position)	
		: mAnimation(locator.GetAnimationManager().Get("up"))
	{
		mAnimation.SetOriginAnchor(sf::Vector2f(0.5f, 0.5f));		
		SetPosition(position);
	}

	sf::FloatRect GetLocalBounds() const override { return mAnimation.GetGlobalBounds(); }

	void Input()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			mDirection.y = -1;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			mDirection.y = 1;
		}
		else
		{
			mDirection.y = 0;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		{
			mDirection.x = 1;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		{
			mDirection.x = -1;
		}
		else
		{
			mDirection.x = 0;
		}
	}

	void Move(const sf::Time& timestamp)
	{
		if (mDirection.lengthSq() != 0)
		{
			mDirection = mDirection.normalized();
		}
		
		MoveX(mDirection.x * timestamp.asSeconds() * mSpeed);
		MoveY(mDirection.y * timestamp.asSeconds() * mSpeed);		
	}

	void Update(const sf::Time& timestamp) override
	{
		Input();
		Move(timestamp);

		mAnimation.Upate(timestamp);
	};

	void Draw(sf::RenderWindow& window) override
	{
		window.draw(mAnimation.GetSprite(), GetTransform());		
	}

private:		
	sf::Vector2f mDirection;	
	float mSpeed{ 200.0f };
	Animation mAnimation;
};