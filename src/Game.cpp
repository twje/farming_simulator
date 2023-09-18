#include <iostream>

#include "Core/IApplicationListener.h"

#include "Level.h"

class Game : public IApplicationListener
{
public:
	void Create() override
	{
		// Load game specific resources
		ResourceLocator().GetTextureManager(); 

		PushLayer(std::make_unique<Level>());
	}
};

std::unique_ptr<IApplicationListener> CreateApplication()
{
	return std::make_unique<Game>();
}