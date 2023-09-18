#pragma once

#include <iostream>

#include "Core/ILayer.h"

class Level : public ILayer
{
public:
	void Create() override
	{
		ResourceLocator& locator = GetResourceLocator();
		std::cout << "Create" << std::endl;
	}

	void Update(const sf::Time& timestamp)
	{
		std::cout << "Update" << std::endl;
	}
};