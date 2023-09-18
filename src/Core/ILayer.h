#pragma once

#include <memory>

#include "Core/IApplicationListener.h"

class ILayer : public IApplicationListener
{
public:
	virtual void Update() = 0;
};