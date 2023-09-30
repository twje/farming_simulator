#include "Core/IApplicationListener.h"
#include "Core/ILayer.h"
#include "Core/LayerStack.h"

void IApplicationListener::PushLayer(std::unique_ptr<ILayer> layer)
{
	// Add resources
	layer->SetLayerStack(mLayerStack);
	layer->SetResourceLocator(mResourceLocator);	
	
	// initialize
	mLayerStack->PushLayer(std::move(layer));	
	mLayerStack->GetTop()->Create();	
}