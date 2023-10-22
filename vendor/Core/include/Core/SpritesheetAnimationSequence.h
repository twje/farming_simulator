#pragma once

#include "Core/Animation.h"
#include "Core/Spritesheet.h"

class SpritesheetAnimationSequence : public AnimationSequence
{
public:
	SpritesheetAnimationSequence(uint16_t sequenceIndex, std::string sequenceId, uint16_t framesPerSecond)
		: AnimationSequence(sequenceIndex, sequenceId, framesPerSecond)
	{
	}

	// AnimationSequence interface
	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) override  { }

	uint16_t GetFrameCount() override { return mFrames.size(); }

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
	std::vector<uint32_t> mFrames;
};