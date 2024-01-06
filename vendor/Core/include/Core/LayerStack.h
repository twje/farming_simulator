#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/ILayer.h"

class LayerStack
{
public:
	ILayer* GetTop() { return mLayers.back().get(); }
	void PushLayer(std::unique_ptr<ILayer> layer) { mLayers.emplace_back(std::move(layer)); }
	void Update(const sf::Time& timestamp);
	void Draw(sf::RenderWindow& window);
	void PostUpdate();
	void OnWindowResize(const sf::Vector2u& size);
	void OnEvent(const sf::Event& event);

private:
	std::vector<std::unique_ptr<ILayer>> mLayers;
};