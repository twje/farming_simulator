#include <iostream>

#include "Core/Application.h"
#include "Core/IApplicationListener.h"
#include "Core/LayerStack.h"
#include "Core/ApplicationConfig.h"

Application::Application(std::unique_ptr<IApplicationListener> listener, ApplicationConfig config)
    : mListener(std::move(listener))
    , mWindow(sf::VideoMode(sf::Vector2u(config.mWidth, config.mHeight), config.mBPP), config.mCaption)
{
    ResourceLocator::GetInstance().Initialize(config);
    mWindow.setVerticalSyncEnabled(true);

	mListener->SetLayerStack(&mLayerStack);	
	mListener->Create();
}

void Application::Run()
{
    const sf::Time timePerFrame = sf::seconds(1.f / 60.f);
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

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
                ApplicationConfig& config = ResourceLocator::GetInstance().GetApplicationConfig();
                config.mHeight = event.size.width;
                config.mWidth = event.size.height;

                mLayerStack.OnWindowResize(config.GetWindowSize());
            }
            mLayerStack.OnEvent(event);
        }
        
        timeSinceLastUpdate += mClock.restart();
        while (timeSinceLastUpdate >= timePerFrame) {
            timeSinceLastUpdate -= timePerFrame;

            mLayerStack.Update(timePerFrame);
            mLayerStack.PostUpdate();
            
            mWindow.clear();
            mLayerStack.Draw(mWindow);
            mWindow.display();
        }
    }
}
