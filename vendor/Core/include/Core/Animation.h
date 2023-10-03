#pragma once

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "Core/Timer.h"
#include "Core/ISerializable.h"
#include "Core/TextureRegion.h"

class AnimationSequence : public ISerializable
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

class Animation : public ISerializable
{
public:
	Animation() = default;
	Animation(const Animation& other);

	void Upate(const sf::Time& timestamp);

	// Getters
	sf::Sprite& GetSprite() const;
	sf::FloatRect GetGlobalBounds() const;

	// Setters
	void SetOriginAnchor(sf::Vector2f originAnchor);
	void AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence);
	void SetAnimationSequence(const std::string& sequenceId);

	// IO
	void SaveToFile(const std::string& filePath);
	void LoadFromFile(const std::string& filePath, AssetManager& assetManager);

	// ISerialize interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
	void RefreshFrame();
	TextureRegion& GetFrame();
	void Animation::SetOriginAnchor(TextureRegion& frame);

private:
	// sequence memebers
	std::unordered_map<std::string, std::shared_ptr<AnimationSequence>> mSequences;
	AnimationSequence* mCurrentSequence{ nullptr };
	TextureRegion mOutSequenceFrame;

	// animation members
	uint16_t mFrameIndex{ 0 };
	Timer mTimer;

	// graphics members
	sf::Vector2f mOriginAnchor;
	std::unique_ptr<sf::Sprite> mSprite;
};