#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/IApplicationListener.h"
#include "Core/LayerStack.h"
#include "Core/ResourceLocator.h"

struct ApplicationConfig
{
	uint16_t mWidth;
	uint16_t mHeight;
	uint16_t mBPP;
	std::string mCaption;
};

class Application
{
public:
	Application(std::unique_ptr<IApplicationListener> listener, ApplicationConfig config);
	
	void Run();

private:
	std::unique_ptr<IApplicationListener> mListener;
	LayerStack mLayerStack;
	ResourceLocator mResourceLocator;
	sf::RenderWindow mWindow;
	sf::Clock mClock;
};