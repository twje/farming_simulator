#include "Core/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

// ----------------------------------------------------------
sf::Vector2f SequenceFrame::GetOrigin(const sf::Vector2f& originAnchor)
{
	float originX = originAnchor.x * mTextureRect.getSize().x;
	float originY = originAnchor.y * mTextureRect.getSize().y;
	return sf::Vector2f(originX, originY);
}

// ----------------------------------------------------------
AnimationSequence::AnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
	: mSequenceId(sequenceId),
	mFramesPerSecond(framesPerSecond),
	mDuration(1.0f / framesPerSecond)
{ }


// ----------------------------------------------------------
Animation::Animation(const Animation& other)
{
	for (const auto& sequencePair : other.mSequences)
	{
		AddAnimationSequence(sequencePair.second);
	}
}

void Animation::Upate(const sf::Time& timestamp)
{
	assert(mCurrentSequence != nullptr);
	mTimer.Update(timestamp);
	if (mTimer.IsFinished())
	{
		mFrameIndex = (mFrameIndex + 1) % mCurrentSequence->GetFrameCount();
		RefreshFrame();
		mTimer.Reset();
	}
}

sf::Sprite& Animation::GetSprite()
{ 
	assert(mSprite != nullptr);
	return *mSprite; 
}

sf::FloatRect Animation::GetGlobalBounds() const
{
	assert(mSprite != nullptr);
	return mSprite->getGlobalBounds();
}

void Animation::SetOriginAnchor(sf::Vector2f originAnchor)
{
	mOriginAnchor = originAnchor;
	auto& frame = GetFrame();
	mSprite->setOrigin(frame.GetOrigin(mOriginAnchor));
}

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

void Animation::SetAnimationSequence(const std::string& sequenceId)
{
	if (mCurrentSequence != nullptr && mCurrentSequence->GetSequenceId() == sequenceId)
	{
		return;
	}

	auto iter = mSequences.find(sequenceId);
	assert(iter != mSequences.end());
	mCurrentSequence = iter->second.get();
	mTimer.Reset(mCurrentSequence->GetDuration());
	mFrameIndex = 0;
	RefreshFrame();
}

void Animation::SaveToFile(const std::string& filePath)
{
	YAML::Emitter emitter;
	Serialize(emitter);
	std::ofstream file(filePath);
	file << emitter.c_str();
}

void Animation::LoadFromFile(const std::string& filePath, ResourceLocator& locator)
{
	YAML::Node node = YAML::LoadFile(filePath);
	Deserialize(node, locator);
}

void Animation::Serialize(YAML::Emitter& emitter)
{
	emitter << YAML::BeginSeq;
	for (const auto& sequence : mSequences)
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "key" << YAML::Value << sequence.first;
		emitter << YAML::Key << "framesPerSecond" << YAML::Value << sequence.second->GetFramesPerSecond();
		emitter << YAML::Key << "frames" << YAML::Value;
		sequence.second->Serialize(emitter);
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;
}

void Animation::Deserialize(const YAML::Node& node, ResourceLocator& locator)
{
	// Define a type alias for AnimationSequence factory functions
	using AnimationSequenceFactory = std::shared_ptr<AnimationSequence>(*)(
		const YAML::Node&, 
		std::string, 
		uint16_t,
		ResourceLocator&
	);
	
	// Factory function for TextureAnimationSequence
	AnimationSequenceFactory textureAnimationSequence = [](
		const YAML::Node& node,
		std::string sequenceId,
		uint16_t framesPerSecond,
		ResourceLocator& locator
		) -> std::shared_ptr<AnimationSequence>
		{
			auto sequence = std::make_unique<TextureAnimationSequence>(sequenceId, framesPerSecond);
			sequence->Deserialize(node, locator);
			return sequence;
		};
	
	// Map of sequence type names to their factory functions
	std::map<std::string, AnimationSequenceFactory> animationSequenceFactories {
		{"TextureAnimationSequence", textureAnimationSequence}
	};

	// Iterate over nodes in the input YAML
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string sequenceId = (*it)["sequenceId"].as<std::string>();
		uint16_t framesPerSecond = (*it)["framesPerSecond"].as<uint16_t>();
		for (const auto& frames : (*it)["frames"])
		{
			std::string sequenceType = frames.first.as<std::string>();
			std::shared_ptr<AnimationSequence> sequence = animationSequenceFactories[sequenceType](
				frames.second,
				sequenceId,
				framesPerSecond,
				locator
			);
			AddAnimationSequence(sequence);
		}
	}
}

void Animation::RefreshFrame()
{
	auto& frame = GetFrame();
	if (mSprite == nullptr)
	{
		mSprite = std::make_unique<sf::Sprite>(*frame.mTexture);
	}
	else
	{
		mSprite->setTexture(*frame.mTexture);
	}
	mSprite->setTextureRect(frame.mTextureRect);
	mSprite->setOrigin(frame.GetOrigin(mOriginAnchor));
}

SequenceFrame& Animation::GetFrame()
{
	assert(mCurrentSequence != nullptr);
	mCurrentSequence->GetFrame(mOutSequenceFrame, mFrameIndex);
	return mOutSequenceFrame;
}