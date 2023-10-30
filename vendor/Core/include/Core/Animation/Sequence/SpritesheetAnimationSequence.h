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

// ------------------------------------------------------------------------
class SpritesheetAnimationSequenceFactory : public AnimationSequenceFactory
{
public:
	SpritesheetAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond, const std::string& spritesheetId,
										const std::vector<uint16_t>& frames)
		: AnimationSequenceFactory(sequenceId, framesPerSecond),
		  mSpritesheetId(spritesheetId),
		  mFrames(frames)
	{ }

	// AnimationSequenceFactory interface
	std::unique_ptr<AnimationSequence> CreateAnimationSequence(AssetManager& assetManager) override
	{
		return nullptr;
	}

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "SpritesheetAnimationSequence" << YAML::Value;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "spritesheetId" << YAML::Value << mSpritesheetId;
		emitter << YAML::Key << "frames" << YAML::Value;
		emitter << YAML::BeginSeq;
		for (const auto& frame : mFrames)
		{
			emitter << frame;
		}
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;
		emitter << YAML::EndMap;
	}

	void Deserialize(const YAML::Node& node) override { }

private:
	std::string mSpritesheetId;
	std::vector<uint16_t> mFrames;
};