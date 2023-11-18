#pragma once

// Includes
//------------------------------------------------------------------------------
// System
#include <string>

// Third party
#include <SFML/Graphics.hpp>

// Project
#include "Core/Animation/AnimationSequence.h"
#include "Core/ISerializable.h"

// Forward Declarations
//--------------------------------------------------------------------------------
class TextureRegion;

//--------------------------------------------------------------------------------
class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond, const std::vector<std::string_view>& frames);

	void ResolveAssetDepsImpl(AssetManager& assetManager) override;
	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) const override;
	uint16_t GetFrameCount() const override { return mFrames.size(); }

	// Serializable methods
	void Serialize(YAML::Emitter& emitter) override;
	static std::unique_ptr<TextureAnimationSequence> Deserialize(const YAML::Node& node);

private:
	std::vector<std::pair<std::string, sf::Texture*>> mFrames;
};