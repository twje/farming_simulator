#include "Core/Animation/AnimationSequence.h"

AnimationSequence::AnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
	: mSequenceId(sequenceId),
	mFramesPerSecond(framesPerSecond),
	mDuration(sf::seconds(1.0f / framesPerSecond))
{ }