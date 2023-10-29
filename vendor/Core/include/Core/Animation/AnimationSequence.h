#pragma once

#include <SFML/Graphics.hpp>

#include "Core/TextureRegion.h"

namespace fs = std::filesystem;

class AnimationSequence
{
public:
	AnimationSequence(std::string sequenceId, uint16_t framesPerSecond);

	// Getters
	const std::string& GetSequenceId() { return mSequenceId; }
	sf::Time GetDuration() { return mDuration; }
	uint16_t GetFramesPerSecond() const { return mFramesPerSecond; }

	// Hooks
	virtual void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) = 0;
	virtual uint16_t GetFrameCount() = 0;

private:
	std::string mSequenceId;
	sf::Time mDuration;
	uint16_t mFramesPerSecond;
};