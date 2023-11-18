#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <string>

// Project
#include "Core/Animation/AnimationSequence.h"

// Forward Declarations
//------------------------------------------------------------------------------
class Spritesheet;
class TextureRegion;

//------------------------------------------------------------------------------
class SpritesheetAnimationSequence : public AnimationSequence
{
public:
	SpritesheetAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, std::string_view spritesheetId, 
								 std::vector<uint16_t>&& frames);

	virtual void ResolveAssetDepsImpl(AssetManager& assetManager) override;
	virtual void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const override;
	virtual uint16_t GetFrameCount() const override;

	// Serializable methods
	void Serialize(YAML::Emitter& emitter) override;
	static std::unique_ptr<SpritesheetAnimationSequence> Deserialize(const YAML::Node& node);

private:
	std::string mSpritesheetId;
	Spritesheet* mSpritesheet;
	std::vector<uint16_t> mFrames;
};