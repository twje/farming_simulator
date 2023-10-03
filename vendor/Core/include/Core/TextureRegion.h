#pragma once

#include <SFML/Graphics.hpp>

class TextureRegion
{
public:
	TextureRegion() = default;
	TextureRegion(sf::Texture* texture, const sf::IntRect& region)
		: mTexture(texture),
		  mRegion(region)
	{ }

	// Setters
	void SetTexture(sf::Texture* texture) { mTexture = texture; }
	void SetRegion(const sf::IntRect& region) { mRegion = region; }

	// Getters
	sf::Texture* GetTexture() const { return mTexture; }
	const sf::IntRect& GetRegion() const { return mRegion; }

private:
	sf::Texture* mTexture{ nullptr };
	sf::IntRect mRegion;
};