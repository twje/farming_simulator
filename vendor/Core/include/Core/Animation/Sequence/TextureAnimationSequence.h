#pragma once

#include <string>
#include <initializer_list>
#include <cassert>

#include "Core/Animation/Factory/AnimationFactory.h"
#include "Core/Animation/AnimationSequence.h"
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
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
		: AnimationSequence(sequenceId, framesPerSecond)
	{ }

	void AddFrames(AssetManager& assetManager, const std::vector<std::string>& frames)
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
	TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond);

	void AddFrames(const std::vector<std::string_view>& frames);

	// AnimationSequenceFactory interface
	std::unique_ptr<AnimationSequence> CreateAnimationSequence(AssetManager& assetManager) override;

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

private:
	std::vector<std::string> mFrames;
};