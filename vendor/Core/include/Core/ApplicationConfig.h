#pragma once

#include <SFML/Graphics.hpp>

struct ApplicationConfig
{
	uint16_t mWidth;
	uint16_t mHeight;
	uint16_t mBPP;
	std::string mCaption;

	sf::Vector2u GetWindowSize() { return sf::Vector2u(mWidth, mHeight); }
};