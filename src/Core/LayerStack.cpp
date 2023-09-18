#include "Core/LayerStack.h"

void LayerStack::Update()
{
	for (auto& layer : mLayers)
	{
		layer->Update();
	}
}