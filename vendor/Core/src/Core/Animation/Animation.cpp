#include "Core/Animation/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"
#include "Core/Animation/AnimationSequence.h"

// ----------------------------------------------------------
Animation::Animation(const Animation& other)
{
	for (const auto& sequencePair : other.mSequences)
	{
		AddAnimationSequence(sequencePair.second);
	}
}

// ----------------------------------------------------------
void Animation::Upate(const sf::Time& timestamp)
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
sf::Sprite& Animation::GetSprite() const
{
	assert(mSprite != nullptr);
	return *mSprite;
}

// ----------------------------------------------------------
sf::FloatRect Animation::GetGlobalBounds() const
{
	assert(mSprite != nullptr);
	return mSprite->getGlobalBounds();
}

// ----------------------------------------------------------
void Animation::SetOriginAnchor(sf::Vector2f originAnchor)
{
	mOriginAnchor = originAnchor;
	SetOriginAnchor(GetFrame());
}

// ----------------------------------------------------------
void Animation::AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence)
{
	const auto& sequenceId = sequence->GetSequenceId();
	assert(mSequences.find(sequenceId) == mSequences.end());

	mSequences[sequenceId] = sequence;
	if (mCurrentSequence == nullptr)
	{
		SetAnimationSequence(sequenceId);
	}
}

// ----------------------------------------------------------
void Animation::SetAnimationSequence(const std::string& sequenceId)
{
	if (mCurrentSequence != nullptr && mCurrentSequence->GetSequenceId() == sequenceId)
	{
		return;
	}

	auto iter = mSequences.find(sequenceId);
	assert(iter != mSequences.end());
	mCurrentSequence = iter->second.get();
	mTimer.SetDuration(mCurrentSequence->GetDuration());
	mTimer.Reset();
	mTimer.Start();
	mFrameIndex = 0;
	RefreshFrame();
}

// ----------------------------------------------------------
void Animation::RefreshFrame()
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
TextureRegion& Animation::GetFrame()
{
	assert(mCurrentSequence != nullptr);
	mCurrentSequence->GetFrame(mOutSequenceFrame, mFrameIndex);
	return mOutSequenceFrame;
}

// ----------------------------------------------------------
void Animation::SetOriginAnchor(TextureRegion& frame)
{
	float originX = mOriginAnchor.x * frame.GetTexture()->getSize().x;
	float originY = mOriginAnchor.y * frame.GetTexture()->getSize().y;
	mSprite->setOrigin(sf::Vector2f(originX, originY));
}