#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

// ------------------------------------------------------------------------
TextureAnimationSequenceFactory::TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond)
	: AnimationSequenceFactory(sequenceId, framesPerSecond)
{ }

void TextureAnimationSequenceFactory::AddFrames(const std::vector<std::string_view>& frames)
{
	mFrames.assign(frames.begin(), frames.end());
}

// ------------------------------------------------------------------------
/*virtual*/ std::unique_ptr<AnimationSequence> TextureAnimationSequenceFactory::CreateAnimationSequence(AssetManager& assetManager)
{
	auto sequence = std::make_unique<TextureAnimationSequence>(GetSequenceId(), GetFramesPerSecond());
	sequence->AddFrames(assetManager, mFrames);
	return sequence;
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
/*virtual*/ void TextureAnimationSequenceFactory::Deserialize(const YAML::Node& node)
{
	std::vector<std::string_view> frames;
	for (const auto& textureId : node["textures"])
	{
		frames.emplace_back(textureId.as<std::string_view>());
	}
	AddFrames(frames);
}