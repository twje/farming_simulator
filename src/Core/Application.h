#pragma once

#include <memory>

#include "Core/IApplicationListener.h"
#include "Core/LayerStack.h"
#include "Core/ResourceLocator.h"

class Application
{
public:
	Application(std::unique_ptr<IApplicationListener> listener);	
	
	void Run();

private:
	std::unique_ptr<IApplicationListener> mListener;
	LayerStack mLayerStack;
	ResourceLocator mResourceLocator;
};