#include "Core/Animation/Sequence/SpritesheetAnimationSequence.h"

// Includes
//------------------------------------------------------------------------------
// project
#include "Core/Spritesheet.h"
#include "Core/TextureRegion.h"

// --------------------------------------------------------------------------------
SpritesheetAnimationSequence::SpritesheetAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, std::string_view spritesheetId, 
														   std::vector<uint16_t>&& frames)
: AnimationSequence(sequenceId, framesPerSecond)
	, mSpritesheetId(spritesheetId)
	, mFrames(std::move(frames))
	, mSpritesheet { nullptr }
{ }

// --------------------------------------------------------------------------------
/*virtual*/ void SpritesheetAnimationSequence::ResolveAssetDepsImpl(AssetManager& assetManager)
{
	mSpritesheet = &assetManager.GetAsset<Spritesheet>(mSpritesheetId);
}

// --------------------------------------------------------------------------------
/*virtual*/ void SpritesheetAnimationSequence::GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const
{
	outFrame = mSpritesheet->GetTextureRegion(mFrames[frameIndex]);
}

// --------------------------------------------------------------------------------
uint16_t SpritesheetAnimationSequence::GetFrameCount() const 
{ 
	return mFrames.size(); 
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationSequence::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "class" << YAML::Value << "SpritesheetAnimationSequence";
	emitter << YAML::Key << "state" << YAML::Value;
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "sequenceId" << GetSequenceId();
	emitter << YAML::Key << "framesPerSecond" << GetFramesPerSecond();
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

// --------------------------------------------------------------------------------
std::unique_ptr<SpritesheetAnimationSequence> SpritesheetAnimationSequence::Deserialize(const YAML::Node& node)
{
	const std::string& sequenceId = node["sequenceId"].as<std::string>();
	uint16_t framesPerSecond = node["framesPerSecond"].as<uint16_t>();
	std::string_view spritesheetId = node["spritesheetId"].as<std::string_view>();

	std::vector<uint16_t> frames;
	for (const auto& textureId : node["frames"])
	{
		frames.emplace_back(textureId.as<uint16_t>());
	}

	return std::make_unique<SpritesheetAnimationSequence>(sequenceId, framesPerSecond, spritesheetId, std::move(frames));
}