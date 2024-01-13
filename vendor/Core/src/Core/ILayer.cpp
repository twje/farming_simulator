// Includes
//------------------------------------------------------------------------------
#include "Core/ILayer.h"

// Core
#include "Core/LayerStack.h"

//------------------------------------------------------------------------------
/*virtual*/ void ILayer::PushLayer(std::unique_ptr<ILayer> layer)
{
	layer->SetLayerStack(GetLayerStack());
	GetLayerStack()->PushLayer(std::move(layer));
}

//------------------------------------------------------------------------------
/*virtual*/ void ILayer::PopLayer()
{
	mIsMarkedForRemoval = true;
	GetLayerStack()->PopLayer(this);
}