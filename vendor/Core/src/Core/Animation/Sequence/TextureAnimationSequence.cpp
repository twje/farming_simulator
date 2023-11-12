#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"
#include "Core/Texture.h"
#include "Core/TextureRegion.h"
#include "Core/AssetManager.h"

// ------------------------------------------------------------------------
TextureAnimationSequence::TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, const std::vector<std::string_view>& frames)
	: AnimationSequence(sequenceId, framesPerSecond)	 
{ 
	for (std::string_view textureId : frames)
	{
		mFrames.push_back(std::make_pair(std::string(textureId), nullptr));
	}
}

// ------------------------------------------------------------------------
/*virtual*/ void TextureAnimationSequence::ResolveAssetDepsImpl(AssetManager& assetManager)
{
	for (auto& frame : mFrames)
	{
		frame.second = &assetManager.GetAsset<Texture>(frame.first).GetRawTexture();
	}
}

// ------------------------------------------------------------------------
void TextureAnimationSequence::GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const
{
	sf::Texture* texture = mFrames[frameIndex].second;
	if (texture != outFrame.GetTexture())
	{
		outFrame.SetTexture(texture);
		outFrame.SetRegion(sf::IntRect(sf::Vector2i(), sf::Vector2i(texture->getSize())));
	}
}

// ------------------------------------------------------------------------
/*virtual*/ void TextureAnimationSequence::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "class" << YAML::Value << "TextureAnimationSequence";
	emitter << YAML::Key << "state" << YAML::Value;
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "sequenceId" << GetSequenceId();
	emitter << YAML::Key << "framesPerSecond" << GetFramesPerSecond();
	emitter << YAML::Key << "textures" << YAML::Value;
	emitter << YAML::BeginSeq;
	for (const auto& frame : mFrames)
	{
		emitter << frame.first;
	}
	emitter << YAML::EndSeq;
	emitter << YAML::EndMap;
	emitter << YAML::EndMap;
}

// ------------------------------------------------------------------------
std::unique_ptr<TextureAnimationSequence> TextureAnimationSequence::Deserialize(const YAML::Node& node)
{
	const std::string& sequenceId = node["sequenceId"].as<std::string>();
	uint16_t framesPerSecond = node["framesPerSecond"].as<uint16_t>();

	std::vector<std::string_view> frames;
	for (const auto& textureId : node["textures"])
	{
		frames.emplace_back(textureId.as<std::string_view>());
	}

	return std::make_unique<TextureAnimationSequence>(sequenceId, framesPerSecond, frames);
}