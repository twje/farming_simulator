#pragma once

#include <SFML/Graphics.hpp>

#include "Core/ISerializable.h"

// Forwardd declarations
class AssetManager;
class TextureRegion;

// --------------------------------------------------------------------------------
class AnimationSequence
{
public:
	AnimationSequence(std::string sequenceId, uint16_t framesPerSecond);

	// Getters
	const std::string& GetSequenceId() const { return mSequenceId; }
	sf::Time GetDuration() const { return mDuration; }
	uint16_t GetFramesPerSecond() const { return mFramesPerSecond; }

	// Hooks
	virtual void ResolveAssetDepsImpl(AssetManager& assetManager) = 0;
	virtual void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const = 0;
	virtual uint16_t GetFrameCount() const = 0;
	virtual void Serialize(YAML::Emitter& emitter) = 0;

private:
	std::string mSequenceId;
	sf::Time mDuration;
	uint16_t mFramesPerSecond;
};
