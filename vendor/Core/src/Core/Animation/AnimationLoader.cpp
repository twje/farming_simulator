#include "Core/Animation/AnimationLoader.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"

// --------------------------------------------------------------------------------
std::unique_ptr<Asset> AnimationLoader::Load(const std::string& filePath)
{
	return Animation::LoadFromFile(filePath);
}