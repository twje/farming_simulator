#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Core/Animation/AnimationSequence.h"
#include "Core/ISerializable.h"

// Forward declarations
class TextureRegion;

// --------------------------------------------------------------------------------
class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, const std::vector<sf::Texture*>& frames);

	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const override;
	uint16_t GetFrameCount() const override { return mFrames.size(); }

private:
	std::vector<sf::Texture*> mFrames;
};

// ------------------------------------------------------------------------
class TextureAnimationSequenceFactory : public AnimationSequenceFactory
{
public:
	TextureAnimationSequenceFactory() = default;
	TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond); // remove

	void PostInit(std::string_view sequenceId, uint16_t framesPerSecond, const std::vector<std::string_view>& frames)
	{
		mSequenceId = sequenceId;
		mFramesPerSecond = framesPerSecond;
		mFrames.assign(frames.begin(), frames.end());
	}

	void AddFrames(const std::vector<std::string_view>& frames);

	// AnimationSequenceFactory interface
	std::unique_ptr<AnimationSequence> CreateAnimationSequence(AssetManager& assetManager) override;

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

private:
	std::vector<std::string> mFrames;
};