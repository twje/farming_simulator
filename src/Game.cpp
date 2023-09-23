#include <iostream>

#include "Core/IApplicationListener.h"

#include "Level.h"

class Game : public IApplicationListener
{
public:
	void Create() override
	{
		auto& locator = GetResourceLocator();
		locator.GetTextureManager().LoadConfig("../../config/textures.cfg");
		locator.GetAnimationManager().LoadConfig("../../config/animations.cfg");

		PushLayer(std::make_unique<Level>());
	}
};

std::unique_ptr<IApplicationListener> CreateApplication()
{
	return std::make_unique<Game>();
}