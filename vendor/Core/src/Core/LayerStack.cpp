#include "Core/LayerStack.h"

void LayerStack::PushLayer(std::unique_ptr<ILayer> layer)
{ 
	mAddLayerList.push_back(std::move(layer));
	if (mLayers.empty())
	{
		AddNewLayers();
	}
}

void LayerStack::PopLayer(ILayer* layer)
{
	mRemoveLayerList.push_back(layer);
}

void LayerStack::Update(const sf::Time& timestamp)
{
	for (auto& layer : mLayers)
	{		
		if (!layer->IsMarkedForRemoval())
		{
			layer->Update(timestamp);	
		}
	}
}

void LayerStack::Draw(sf::RenderWindow& window)
{
	for (auto& layer : mLayers)
	{		
		if (!layer->IsMarkedForRemoval())
		{
			layer->Draw(window);	
		}
	}
}

void LayerStack::PostUpdate()
{
	AddNewLayers();
	RemoveLayers();
	PostUpdateLayers();
}

void LayerStack::OnWindowResize(const sf::Vector2u& size)
{
	for (auto& layer : mLayers)
	{
		if (!layer->IsMarkedForRemoval())
		{
			layer->OnWindowResize(size);
		}
	}
}

void LayerStack::OnEvent(const sf::Event& event)
{
	for (auto& layer : mLayers)
	{
		if (!layer->IsMarkedForRemoval())
		{
			layer->OnEvent(event);
		}
	}
}

void LayerStack::AddNewLayers()
{
	for (auto& layerPtr : mAddLayerList)
	{
		layerPtr->Create();
		mLayers.push_back(std::move(layerPtr));
	}
	mAddLayerList.clear();
}

void LayerStack::RemoveLayers()
{
	for (ILayer* layer : mRemoveLayerList)
	{
		auto it = std::find_if(mLayers.begin(), mLayers.end(),
			[layer](const std::unique_ptr<ILayer>& layerPtr) {
				return layerPtr.get() == layer;
			});

		if (it != mLayers.end()) {
			mLayers.erase(it);
		}
	}
	mRemoveLayerList.clear();
}

void LayerStack::PostUpdateLayers()
{
	for (auto& layer : mLayers)
	{		
		if (!layer->IsMarkedForRemoval())
		{
			layer->PostUpdate();		
		}
	}
}