#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"
#include "Core/TextureRegion.h"
#include "Core/AssetManager.h"

// ------------------------------------------------------------------------
TextureAnimationSequence::TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
	: AnimationSequence(sequenceId, framesPerSecond)
{ }

// ------------------------------------------------------------------------
void TextureAnimationSequence::AddFrames(AssetManager& assetManager, const std::vector<std::string>& frames)
{
	for (const std::string& textureId : frames)
	{
		AddFrame(assetManager, textureId);
	}
}

// ------------------------------------------------------------------------
void TextureAnimationSequence::GetFrame(TextureRegion& outFrame, uint16_t frameIndex)
{
	sf::Texture* texture = mFrames[frameIndex];
	if (texture != outFrame.GetTexture())
	{
		outFrame.SetTexture(texture);
		outFrame.SetRegion(sf::IntRect(sf::Vector2i(), sf::Vector2i(texture->getSize())));
	}
}

// ------------------------------------------------------------------------
void TextureAnimationSequence::TextureAnimationSequence::AddFrame(AssetManager& assetManager, std::string textureId)
{
	sf::Texture& texture = assetManager.GetAsset<sf::Texture>(textureId);
	mFrames.emplace_back(&texture);
}

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