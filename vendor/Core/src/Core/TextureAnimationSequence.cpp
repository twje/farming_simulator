#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

void TextureAnimationSequence::Deserialize(const YAML::Node& node, AssetManager& assetManager)
{
	for (const auto& textureId : node["textures"])
	{
		AddFrame(assetManager, textureId.as<std::string>());
	}
}