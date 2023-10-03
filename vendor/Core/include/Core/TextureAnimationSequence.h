#pragma once

#include <string>
#include <initializer_list>

#include "Core/Animation.h"
#include "Core/AssetManager.h"

class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
		: AnimationSequence(sequenceId, framesPerSecond)
	{
	}

	void AddFrames(AssetManager& assetManager, std::initializer_list<std::string> frames)
	{
		for (const std::string& textureId : frames)
		{
			AddFrame(assetManager, textureId);
		}
	}

	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) override
	{
		sf::Texture* texture = mFrames.at(frameIndex).second;
		if (texture != outFrame.GetTexture())
		{
			outFrame.SetTexture(texture);
			outFrame.SetRegion(sf::IntRect(sf::Vector2i(), sf::Vector2i(texture->getSize())));
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

	void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
	void TextureAnimationSequence::AddFrame(AssetManager& assetManager, std::string textureId)
	{
		sf::Texture& texture = assetManager.GetAsset<sf::Texture>(textureId);
		auto frame = std::make_pair(textureId, &texture);
		mFrames.emplace_back(frame);
	}

private:
	std::vector<std::pair<std::string, sf::Texture*>> mFrames;
};