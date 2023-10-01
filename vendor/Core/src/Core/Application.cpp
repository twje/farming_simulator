#include <iostream>

#include "Core/Application.h"
#include "Core/IApplicationListener.h"
#include "Core/LayerStack.h"
#include "Core/ApplicationConfig.h"

Application::Application(std::unique_ptr<IApplicationListener> listener, ApplicationConfig config)
	: mListener(std::move(listener)),
	  mWindow(sf::VideoMode(sf::Vector2u(config.mWidth, config.mHeight), config.mBPP), config.mCaption),
      mResourceLocator(config)
{
	mListener->SetLayerStack(&mLayerStack);
	mListener->SetResourceLocator(&mResourceLocator);
	mListener->Create();
}

void Application::Run()
{
    while (mWindow.isOpen())
    {
        sf::Event event;
        while (mWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                mWindow.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                mResourceLocator.mConfig.mHeight = event.size.width;
                mResourceLocator.mConfig.mWidth = event.size.height;
                mLayerStack.OnWindowResize(mResourceLocator.mConfig.GetWindowSize());
            }
            mLayerStack.OnEvent(event);
        }

        mLayerStack.Update(mClock.restart());

        mWindow.clear();
        mLayerStack.Draw(mWindow);
        mWindow.display();

        mLayerStack.EndFrame();
    }
}
