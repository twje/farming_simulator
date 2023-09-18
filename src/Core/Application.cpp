#include "Core/Application.h"

#include "Core/IApplicationListener.h"
#include "Core/LayerStack.h"

Application::Application(std::unique_ptr<IApplicationListener> listener)
	: mListener(std::move(listener))
{	
	mResourceLocator.SetTextureManager(std::make_unique<TextureManager>());

	mListener->SetLayerStack(&mLayerStack);
	mListener->SetResourceLocator(&mResourceLocator);
	mListener->Create();
}

void Application::Run()
{	
	mLayerStack.Update();
}
