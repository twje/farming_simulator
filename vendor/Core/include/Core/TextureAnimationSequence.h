#pragma once

#include <string>
#include <initializer_list>

#include "Core/Animation.h"
#include "Core/TextureManager.h"

class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
		: AnimationSequence(sequenceId, framesPerSecond)
	{
	}

	void AddFrames(TextureManager& textureManager, std::initializer_list<std::string> frames)
	{
		for (const std::string& textureId : frames)
		{
			AddFrame(textureManager, textureId);
		}
	}

	void GetFrame(SequenceFrame& outFrame, uint16_t frameIndex) override
	{
		sf::Texture* texture = mFrames.at(frameIndex).second;
		if (texture != outFrame.mTexture)
		{
			outFrame.mTexture = texture;
			outFrame.mTextureRect = sf::IntRect(sf::Vector2i(), sf::Vector2i(outFrame.mTexture->getSize()));
		}
	}

	virtual uint16_t GetFrameCount() { return mFrames.size(); }

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "TextureAnimationSequence" << YAML::Value;
		emitter << YAML::BeginMap;
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

	void Deserialize(const YAML::Node& node, ResourceLocator& locator) override;

private:
	void TextureAnimationSequence::AddFrame(TextureManager& textureManager, std::string textureId)
	{
		auto frame = std::make_pair(textureId, &textureManager.Get(textureId));
		mFrames.emplace_back(frame);
	}

private:
	std::vector<std::pair<std::string, sf::Texture*>> mFrames;
};