#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"
#include "Core/Texture.h"
#include "Core/TextureRegion.h"
#include "Core/AssetManager.h"

// ------------------------------------------------------------------------
TextureAnimationSequence::TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, const std::vector<sf::Texture*>& frames)
	: AnimationSequence(sequenceId, framesPerSecond),
	  mFrames(frames)
{ }

// ------------------------------------------------------------------------
void TextureAnimationSequence::GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const
{
	sf::Texture* texture = mFrames[frameIndex];
	if (texture != outFrame.GetTexture())
	{
		outFrame.SetTexture(texture);
		outFrame.SetRegion(sf::IntRect(sf::Vector2i(), sf::Vector2i(texture->getSize())));
	}
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
	std::vector<sf::Texture*> textureFrames;

	for (const std::string& textureId : mFrames)
	{
		textureFrames.emplace_back(&assetManager.GetAsset<Texture>(textureId).GetRawTexture());
	}

	return std::make_unique<TextureAnimationSequence>(GetSequenceId(), GetFramesPerSecond(), textureFrames);
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