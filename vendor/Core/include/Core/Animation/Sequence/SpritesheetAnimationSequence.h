#pragma once

#include "Core/Animation/AnimationSequence.h"
#include "Core/Spritesheet.h"

class SpritesheetAnimationSequence : public AnimationSequence
{
public:
	SpritesheetAnimationSequence(uint16_t sequenceIndex, std::string sequenceId, uint16_t framesPerSecond);

	// AnimationSequence interface
	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const override  { }

	uint16_t GetFrameCount() const override { return mFrames.size(); }

private:
	std::vector<uint32_t> mFrames;
};