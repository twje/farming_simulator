#pragma once

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/RectUtils.h"

#include "Player.h"
#include "Settings.h"

class Overlay : public IPlayerObserver
{
public:
	Overlay(AssetManager& assetManager, Player& player)
		: mAssetManager(assetManager),
		  mPlayer(player),
		  mToolSprite(assetManager.GetAsset<sf::Texture>(player.GetActiveTool())),
		  mSeedSprite(assetManager.GetAsset<sf::Texture>(player.GetActiveSeed()))
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
		sf::Texture& texture = mAssetManager.GetAsset<sf::Texture>(textureId);
		sprite.setTexture(texture, true);
		sprite.setOrigin(GetRectMidBottom(sprite.getLocalBounds()));
		sprite.setPosition(OVERLAY_POSITIONS.at(overlayId));
	}

private:
	AssetManager& mAssetManager;
	Player& mPlayer;
	sf::Sprite mToolSprite;
	sf::Sprite mSeedSprite;
};