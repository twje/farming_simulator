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
	virtual void PushLayer(std::unique_ptr<ILayer> layer) = 0;
	virtual void PopLayer() = 0;

	// Setters
	void SetLayerStack(LayerStack* layerStack) { mLayerStack = layerStack; }	

	// Getters
	ResourceLocator& GetResourceLocator() const 
	{ 
		return ResourceLocator::GetInstance();
	}

	LayerStack* GetLayerStack() { return mLayerStack; }

private:
	LayerStack* mLayerStack;	
};