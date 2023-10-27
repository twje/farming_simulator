#pragma once

#include <string>
#include <initializer_list>
#include <cassert>

#include "Core/AnimationFactory.h"
#include "Core/Animation.h"
#include "Core/AssetManager.h"
#include "Core/ISerializable.h"

// --------------------------------------------------------------------------------
class TextureAnimationSequenceFrame
{
public:
	TextureAnimationSequenceFrame(std::string textureId, sf::Texture* texture)
		: mTextureId(textureId),
		  mTexture(texture)
	{ }

	const std::string& GetTextureId() const { return mTextureId; }
	sf::Texture* GetTexture() { return mTexture; }

private:
	std::string mTextureId;
	sf::Texture* mTexture;
};

// --------------------------------------------------------------------------------
class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(uint16_t sequenceIndex, std::string sequenceId, uint16_t framesPerSecond)
		: AnimationSequence(sequenceIndex, sequenceId, framesPerSecond)
	{ }

	void AddFrames(AssetManager& assetManager, std::initializer_list<std::string> frames)
	{
		for (const std::string& textureId : frames)
		{
			AddFrame(assetManager, textureId);
		}
	}

	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) override
	{
		sf::Texture* texture = mFrames.at(frameIndex).GetTexture();
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
			emitter << frame.GetTextureId();
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
		mFrames.emplace_back(textureId, &texture);
	}

private:
	std::vector<TextureAnimationSequenceFrame> mFrames;
};

// ------------------------------------------------------------------------
class TextureAnimationSequenceFactory : public AnimationSequenceFactory
{
public:
	TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond,
		                            std::initializer_list<std::string_view> frames);

	// AnimationSequenceFactory interface
	std::unique_ptr<Animation> CreateAnimationSequence() override;

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
	std::vector<std::string> mFrames;
};