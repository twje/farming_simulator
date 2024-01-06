#include "Core/LayerStack.h"

void LayerStack::Update(const sf::Time& timestamp)
{
	for (auto& layer : mLayers)
	{
		layer->Update(timestamp);
	}
}

void LayerStack::Draw(sf::RenderWindow& window)
{
	for (auto& layer : mLayers)
	{
		layer->Draw(window);
	}
}

void LayerStack::PostUpdate()
{
	for (auto& layer : mLayers)
	{
		layer->PostUpdate();
	}
}

void LayerStack::OnWindowResize(const sf::Vector2u& size)
{
	for (auto& layer : mLayers)
	{
		layer->OnWindowResize(size);
	}
}

void LayerStack::OnEvent(const sf::Event& event)
{
	for (auto& layer : mLayers)
	{
		layer->OnEvent(event);
	}
}