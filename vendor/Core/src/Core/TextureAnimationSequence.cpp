#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

// ------------------------------------------------------------------------
void TextureAnimationSequence::Deserialize(const YAML::Node& node, AssetManager& assetManager)
{
	for (const auto& textureId : node["textures"])
	{
		AddFrame(assetManager, textureId.as<std::string>());
	}
}

// ------------------------------------------------------------------------
TextureAnimationSequenceFactory::TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond,
																 std::initializer_list<std::string_view> frames)
	: AnimationSequenceFactory(sequenceId, framesPerSecond),
	  mFrames(frames.begin(), frames.end())
{ }

// ------------------------------------------------------------------------
/*virtual*/ std::unique_ptr<Animation> TextureAnimationSequenceFactory::CreateAnimationSequence()
{
	return nullptr;
}

// ------------------------------------------------------------------------
/*virtual*/ void TextureAnimationSequenceFactory::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "TextureAnimationSequence" << YAML::Value;
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "textures" << YAML::Value;
	emitter << YAML::BeginSeq;
	for (const auto& frame : mFrames)
	{
		emitter << frame;
	}
	emitter << YAML::EndSeq;
	emitter << YAML::EndMap;
	emitter << YAML::EndMap;
}

// ------------------------------------------------------------------------
/*virtual*/ void TextureAnimationSequenceFactory::Deserialize(const YAML::Node& node, AssetManager& assetManager)
{

}