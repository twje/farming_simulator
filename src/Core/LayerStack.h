#pragma once

#include <vector>
#include <memory>

#include "Core/ILayer.h"

class LayerStack
{
public:	
	ILayer* GetTop() { return mLayers.back().get(); }
	void PushLayer(std::unique_ptr<ILayer> layer) { mLayers.emplace_back(std::move(layer)); }
	void Update();

private:	
	std::vector<std::unique_ptr<ILayer>> mLayers;
};