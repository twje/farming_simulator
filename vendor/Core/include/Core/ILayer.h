#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/IApplicationListener.h"

class ILayer : public IApplicationListener
{
public:
	virtual void Update(const sf::Time& timestamp) { }
	virtual void Draw(sf::RenderWindow& window) { }
	virtual void EndFrame() { }
	virtual void OnWindowResize(const sf::Vector2u& size) { }
	virtual void OnEvent(const sf::Event& event) { }
};