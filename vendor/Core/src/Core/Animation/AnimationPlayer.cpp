#include "Core/Animation/AnimationPlayer.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"
#include "Core/Animation/Sequence/TextureAnimationSequence.h"

// ----------------------------------------------------------
AnimationPlayer::AnimationPlayer(const Animation& animation)
	: mAnimation(animation)
{
	SetAnimationSequence(mAnimation.GetStartSequenceId());
}

// ----------------------------------------------------------
void AnimationPlayer::Upate(const sf::Time& timestamp)
{
	assert(mCurrentSequence != nullptr);
	mTimer.Update(timestamp);
	if (mTimer.IsFinished())
	{
		mFrameIndex = (mFrameIndex + 1) % mCurrentSequence->GetFrameCount();
		RefreshFrame();
		mTimer.Reset();
		mTimer.Start();
	}
}

// ----------------------------------------------------------
sf::Sprite& AnimationPlayer::GetSprite() const
{
	assert(mSprite != nullptr);
	return *mSprite;
}

// ----------------------------------------------------------
sf::FloatRect AnimationPlayer::GetGlobalBounds() const
{
	assert(mSprite != nullptr);
	return mSprite->getGlobalBounds();
}

// ----------------------------------------------------------
void AnimationPlayer::SetOriginAnchor(sf::Vector2f originAnchor)
{
	mOriginAnchor = originAnchor;
	SetOriginAnchor(GetFrame());
}

// ----------------------------------------------------------
void AnimationPlayer::SetAnimationSequence(const std::string& sequenceId)
{
	if (mCurrentSequence != nullptr && mCurrentSequence->GetSequenceId() == sequenceId)
	{
		return;
	}

	mCurrentSequence = &mAnimation.GetSequence(sequenceId);
	mTimer.SetDuration(mCurrentSequence->GetDuration());
	mTimer.Reset();
	mTimer.Start();
	mFrameIndex = 0;
	RefreshFrame();
}

// ----------------------------------------------------------
TextureRegion& AnimationPlayer::GetFrame()
{
	assert(mCurrentSequence != nullptr);
	mCurrentSequence->GetFrame(mOutSequenceFrame, mFrameIndex);
	return mOutSequenceFrame;
}

// ----------------------------------------------------------
void AnimationPlayer::RefreshFrame()
{
	auto& frame = GetFrame();
	if (mSprite == nullptr)
	{
		mSprite = std::make_unique<sf::Sprite>(*frame.GetTexture());
	}
	else
	{
		mSprite->setTexture(*frame.GetTexture());
	}
	mSprite->setTextureRect(frame.GetRegion());
	SetOriginAnchor(frame);
}

// ----------------------------------------------------------
void AnimationPlayer::SetOriginAnchor(TextureRegion& frame)
{
	float originX = mOriginAnchor.x * frame.GetTexture()->getSize().x;
	float originY = mOriginAnchor.y * frame.GetTexture()->getSize().y;
	mSprite->setOrigin(sf::Vector2f(originX, originY));
}
