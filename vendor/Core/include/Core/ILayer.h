#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/IApplicationListener.h"

class ILayer : public IApplicationListener
{
public:
	bool IsMarkedForRemoval() { return mIsMarkedForRemoval; }
	
	// IApplicationListener interface
	virtual void PushLayer(std::unique_ptr<ILayer> layer) override;
	virtual void PopLayer() override;

	// Hooks
	virtual void Update(const sf::Time& timestamp) { }
	virtual void Draw(sf::RenderWindow& window) { }
	virtual void PostUpdate() { }
	virtual void OnWindowResize(const sf::Vector2u& size) { }
	virtual void OnEvent(const sf::Event& event) { }
	
	// Mandatory if derived from IApplicationListener (entry point)
	virtual void Create() override { }

private:
	bool mIsMarkedForRemoval{ false };
};