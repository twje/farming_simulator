#pragma once

#include <unordered_map>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "Core/Timer.h"
#include "Core/TextureRegion.h"

namespace fs = std::filesystem;

// Forward declaration
class AnimationSequence;

class Animation
{
	using SequenceMap = std::unordered_map<std::string, std::shared_ptr<AnimationSequence>>;
	using SequencePair = std::pair<std::string, std::shared_ptr<AnimationSequence>>;

public:
	Animation() = default;
	Animation(const Animation& other);

	void Upate(const sf::Time& timestamp);

	// Getters
	sf::Sprite& GetSprite() const;
	sf::FloatRect GetGlobalBounds() const;
	const SequenceMap& GetAnimationSequences() const { return mSequences; }

	// Setters
	void SetOriginAnchor(sf::Vector2f originAnchor);
	void AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence);
	void SetAnimationSequence(const std::string& sequenceId);

private:
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