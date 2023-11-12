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
	TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond, 
									const std::vector<std::string_view>& frames);	

	// AnimationSequenceFactory interface
	std::unique_ptr<AnimationSequence> CreateAnimationSequence(AssetManager& assetManager) override;

	// Serializable methods
	void Serialize(YAML::Emitter& emitter) override;
	static std::unique_ptr<TextureAnimationSequenceFactory> Deserialize(const YAML::Node& node);

private:
	std::vector<std::string> mFrames;
};