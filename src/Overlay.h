#pragma once

#include <SFML/Graphics.hpp>

#include "Core/ResourceLocator.h"
#include "Core/RectUtils.h"

#include "Player.h"
#include "Settings.h"

class Overlay : public IPlayerObserver
{
public:
	Overlay(ResourceLocator& resourceLocator, Player& player)
		: mResourceLocator(resourceLocator),
		  mPlayer(player),
		  mToolSprite(resourceLocator.GetTextureManager().Get(player.GetActiveTool())),
		  mSeedSprite(resourceLocator.GetTextureManager().Get(player.GetActiveSeed()))
	{
		SetOverlayTexture(player.GetActiveTool(), "tool", mToolSprite);
		SetOverlayTexture(player.GetActiveSeed(), "seed", mSeedSprite);
		player.Subscribe(this);
	}
	
	void Draw(sf::RenderWindow& window)
	{
		window.draw(mToolSprite);
		window.draw(mSeedSprite);
	}

private:
	void ToolChanged(std::string tool) override
	{
		SetOverlayTexture(tool, "tool", mToolSprite);
	}

	void SeedChanged(std::string seed) override
	{
		SetOverlayTexture(seed, "seed", mSeedSprite);
	}

	void SetOverlayTexture(const std::string& textureId, const std::string& overlayId, sf::Sprite& sprite)
	{
		sf::Texture& texture = mResourceLocator.GetTextureManager().Get(textureId);
		sprite.setTexture(texture, true);
		sprite.setOrigin(GetRectMidBottom(sprite.getLocalBounds()));		
		sprite.setPosition(OVERLAY_POSITIONS.at(overlayId));
	}	

private:
	ResourceLocator& mResourceLocator;
	Player& mPlayer;
	sf::Sprite mToolSprite;
	sf::Sprite mSeedSprite;
};