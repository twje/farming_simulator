#pragma once

#include <string>

#include "Core/Animation/AnimationSequence.h"
#include "Core/ISerializable.h"

// Forward declarations
class AssetManager;
class TextureRegion;

// --------------------------------------------------------------------------------
class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(std::string sequenceId, uint16_t framesPerSecond);

	void AddFrames(AssetManager& assetManager, const std::vector<std::string>& frames);
	void GetFrame(TextureRegion& outFrame, uint16_t frameIndex) override;
	virtual uint16_t GetFrameCount() { return mFrames.size(); }

private:
	void TextureAnimationSequence::AddFrame(AssetManager& assetManager, std::string textureId);

private:
	std::vector<sf::Texture*> mFrames;
};

// ------------------------------------------------------------------------
class TextureAnimationSequenceFactory : public AnimationSequenceFactory
{
public:
	TextureAnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond);

	void AddFrames(const std::vector<std::string_view>& frames);

	// AnimationSequenceFactory interface
	std::unique_ptr<AnimationSequence> CreateAnimationSequence(AssetManager& assetManager) override;

	// ISerializable interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

private:
	std::vector<std::string> mFrames;
};