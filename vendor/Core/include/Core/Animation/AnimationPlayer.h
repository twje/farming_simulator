#pragma once

#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/Timer.h"
#include "Core/TextureRegion.h"

// Forward declaration
class Animation;
class AnimationSequence;

class AnimationPlayer
{
	using SequenceMap = std::unordered_map<std::string, std::shared_ptr<AnimationSequence>>;
	using SequencePair = std::pair<std::string, std::shared_ptr<AnimationSequence>>;

public:
	AnimationPlayer(const Animation& animation);

	void Upate(const sf::Time& timestamp);

	// Getters
	sf::Sprite& GetSprite() const;
	sf::FloatRect GetGlobalBounds() const;

	// Setters
	void SetOriginAnchor(sf::Vector2f originAnchor);
	void SetAnimationSequence(const std::string& sequenceId);

private:
	TextureRegion& GetFrame();
	void RefreshFrame();
	void SetOriginAnchor(TextureRegion& frame);

private:
	const Animation& mAnimation;

	// sequence memebers
	const AnimationSequence* mCurrentSequence{ nullptr };
	TextureRegion mOutSequenceFrame;

	// animation members
	uint16_t mFrameIndex{ 0 };
	Timer mTimer;

	// graphics members
	sf::Vector2f mOriginAnchor;
	std::unique_ptr<sf::Sprite> mSprite;
};