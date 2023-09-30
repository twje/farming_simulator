#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

void TextureAnimationSequence::Deserialize(const YAML::Node& node, ResourceLocator& locator)
{
	for (const auto& textureId : node["textures"])
	{
		AddFrame(locator.GetTextureManager(), textureId.as<std::string>());
	}
}