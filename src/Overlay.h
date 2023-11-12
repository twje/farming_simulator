#pragma once

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/RectUtils.h"
#include "Core/Texture.h"

#include "Player.h"
#include "Settings.h"

class Overlay : public IPlayerObserver
{
public:
	Overlay(AssetManager& assetManager, Player& player)
		: mAssetManager(assetManager),
		  mPlayer(player),
		  mToolSprite(assetManager.GetAsset<Texture>(player.GetActiveTool()).GetRawTexture()),
		  mSeedSprite(assetManager.GetAsset<Texture>(player.GetActiveSeed()).GetRawTexture())
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
		const sf::Texture& texture = mAssetManager.GetAsset<Texture>(textureId).GetRawTexture();
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