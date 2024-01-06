#pragma once

#include <memory>

#include "Core/ResourceLocator.h"

// Forward declaration
class ILayer;
class LayerStack;

class IApplicationListener
{
public:
	virtual ~IApplicationListener() = default;

	virtual void Create() = 0;
	void PushLayer(std::unique_ptr<ILayer> layer);

	// Setters
	void SetLayerStack(LayerStack* layerStack) { mLayerStack = layerStack; }	

	// Getters
	ResourceLocator& GetResourceLocator() const 
	{ 
		return ResourceLocator::GetInstance();
	}

private:
	LayerStack* mLayerStack;	
};