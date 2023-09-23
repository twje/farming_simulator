#pragma once

#include <iostream>

#include "Core/GameObject.h"
#include "Core/ResourceLocator.h"

class Player : public Sprite
{
public:
	Player(ResourceLocator& locator, const sf::Vector2f& position)		
	{
		auto& textureManager = locator.GetTextureManager();
		mSprite = std::make_unique<sf::Sprite>(textureManager.Get("player"));
		mSprite->setPosition(-.5f * mSprite->getLocalBounds().getSize());
		SetPosition(position);
	}

	sf::FloatRect GetLocalBounds() const override { return mSprite->getGlobalBounds(); }

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
	};

	void Draw(sf::RenderWindow& window) override
	{
		window.draw(*mSprite, GetTransform());
	}

private:		
	sf::Vector2f mDirection;
	std::unique_ptr<sf::Sprite> mSprite;	
	float mSpeed{ 200.0f };
};