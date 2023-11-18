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
public:
	AnimationPlayer(const Animation& animation);

	void Upate(const sf::Time& timestamp);

	// Getters
	sf::Sprite& GetSprite() const;
	sf::FloatRect GetGlobalBounds() const;

	// Setters	
	void SetAnimationSequence(const std::string& sequenceId);

private:
	TextureRegion& GetFrame();
	void RefreshFrame();	

private:
	const Animation& mAnimation;

	// sequence memebers
	const AnimationSequence* mCurrentSequence{ nullptr };
	TextureRegion mOutSequenceFrame;

	// animation members
	uint16_t mFrameIndex{ 0 };
	Timer mTimer;

	// graphics members	
	std::unique_ptr<sf::Sprite> mSprite;
};