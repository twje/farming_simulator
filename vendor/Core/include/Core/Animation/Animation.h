#pragma once

#include <unordered_map>
#include <filesystem>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/ISerializable.h"
#include "Core/Timer.h"
#include "Core/TextureRegion.h"

namespace fs = std::filesystem;

// Forward declaration
class AnimationSequence;
class AnimationSequenceFactory;

// --------------------------------------------------------------------------------
class Animation
{
	using SequenceMap = std::unordered_map<std::string, std::shared_ptr<AnimationSequence>>;
	using SequencePair = std::pair<std::string, std::shared_ptr<AnimationSequence>>;

public:
	Animation(const std::vector<std::shared_ptr<AnimationSequence>>& sequences);
	Animation(const Animation& other);

	void Upate(const sf::Time& timestamp);

	// Getters
	sf::Sprite& GetSprite() const;
	sf::FloatRect GetGlobalBounds() const;
	const SequenceMap& GetAnimationSequences() const { return mSequences; }

	// Setters
	void SetOriginAnchor(sf::Vector2f originAnchor);
	void SetAnimationSequence(const std::string& sequenceId);

private:
	void AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence);
	void RefreshFrame();
	TextureRegion& GetFrame();
	void Animation::SetOriginAnchor(TextureRegion& frame);

private:
	// sequence memebers
	SequenceMap mSequences;
	AnimationSequence* mCurrentSequence{ nullptr };
	TextureRegion mOutSequenceFrame;

	// animation members
	uint16_t mFrameIndex{ 0 };
	Timer mTimer;

	// graphics members
	sf::Vector2f mOriginAnchor;
	std::unique_ptr<sf::Sprite> mSprite;
};

// --------------------------------------------------------------------------------
class AnimationFactory : public ISerializable
{
public:
	std::unique_ptr<Animation> CreateAnimation(AssetManager& assetManager);

	void AddAnimationSequenceFactory(std::unique_ptr<AnimationSequenceFactory> factory);

	// IO
	void SaveToFile(const std::string& filePath);
	void LoadFromFile(const std::string& filePath);

	// ISerialize interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

private:
	std::vector<std::unique_ptr<AnimationSequenceFactory>> mSequenceFactories;
};