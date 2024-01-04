#pragma once

#include "SFML/Graphics.hpp"

sf::FloatRect InflateRect(const sf::FloatRect& source, float inflateX, float inflateY);
sf::Vector2f GetRectCenter(const sf::FloatRect& rect);
sf::Vector2f GetRectMidRight(const sf::FloatRect& rect);
sf::Vector2f GetRectMidLeft(const sf::FloatRect& rect);
sf::Vector2f GetRectMidTop(const sf::FloatRect& rect);
sf::Vector2f GetRectMidBottom(const sf::FloatRect& rect);

template<typename T>
void DrawRect(sf::RenderTarget& target, const T& rect, const sf::Color& color)
{
	sf::RectangleShape rectangleShape;
	rectangleShape.setSize(rect.getSize());
	rectangleShape.setPosition(rect.getPosition());
	rectangleShape.setFillColor({ 0, 0, 0, 0 });
	rectangleShape.setOutlineColor(color);
	rectangleShape.setOutlineThickness(-1);

	target.draw(rectangleShape);
}