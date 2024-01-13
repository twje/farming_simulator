#pragma once

// Includes
//------------------------------------------------------------------------------
// Game
#include "Player.h"

// Core
#include "Core/ILayer.h"
#include "Core/Timer.h"
#include "Core/ResourceLocator.h"

// Third party
#include "SFML/Graphics.hpp"

// System
#include <iostream>
#include <functional>

//------------------------------------------------------------------------------
class Transition : public ILayer
{
public:
	Transition(Player& player, std::function<void()> resetCallback)
		: mPlayer(player)
		, mResetCallback(resetCallback)
		, mTimer(sf::seconds(2.0f))
		, mCurrentColor({255, 255, 255, 0})
	{					
		mTimer.Start();
		UpdateOverlay();
	}

	virtual void Update(const sf::Time& timestamp) 
	{
		mTimer.Update(timestamp);
		if (mTimer.IsFinished())
		{
			mPlayer.WakeUp();
			mResetCallback();
			PopLayer();
		}
		else
		{
			float percent = std::min(1.0f, mTimer.PercentComplete() * 2.0f);
			if (mTimer.PercentComplete() > 0.5f)
			{
				percent = std::max(0.0f, (1.0f - mTimer.PercentComplete()) * 2.0f);
			}
			mCurrentColor.a = static_cast<uint8_t>(percent * 255);

			UpdateOverlay();
		}
	}

	virtual void Draw(sf::RenderWindow& window) 
	{ 
		window.draw(mOverlay);
	}

private:
	void UpdateOverlay()
	{
		ApplicationConfig& config = ResourceLocator::GetInstance().GetApplicationConfig();
		mOverlay.setSize(sf::Vector2f(config.mWidth, config.mHeight));
		mOverlay.setFillColor(mCurrentColor);
	}

	sf::RectangleShape mOverlay;
	sf::Color mCurrentColor;
	Player& mPlayer;
	Timer mTimer;
	std::function<void()> mResetCallback;
};