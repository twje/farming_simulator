#include "Core/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

// ----------------------------------------------------------
AnimationSequence::AnimationSequence(uint32_t sequenceIndex, std::string sequenceId, uint16_t framesPerSecond)
	: mSequenceIndex(sequenceIndex),
	  mSequenceId(sequenceId),
	  mFramesPerSecond(framesPerSecond),
	  mDuration(sf::seconds(1.0f / framesPerSecond))
{ }

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
void Animation::SaveToFile(const std::string& filePath)
{
	YAML::Emitter emitter;
	Serialize(emitter);
	std::ofstream file(filePath);
	file << emitter.c_str();
}

// ----------------------------------------------------------
void Animation::LoadFromFile(const std::string& filePath, AssetManager& assetManager)
{
	YAML::Node node = YAML::LoadFile(filePath);
	Deserialize(node, assetManager);
}

// ----------------------------------------------------------
void Animation::Serialize(YAML::Emitter& emitter)
{
	std::vector<SequencePair> sortedSequences;
	SortSequencesByIndex(sortedSequences);

	// Serialize
	emitter << YAML::BeginSeq;
	for (const auto& sequence : sortedSequences)
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "sequenceId" << YAML::Value << sequence.first;
		emitter << YAML::Key << "framesPerSecond" << YAML::Value << sequence.second->GetFramesPerSecond();
		emitter << YAML::Key << "frames" << YAML::Value;
		sequence.second->Serialize(emitter);
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;
}

// ----------------------------------------------------------
void Animation::Deserialize(const YAML::Node& node, AssetManager& assetManager)
{
	// Define a type alias for AnimationSequence factory functions
	using AnimationSequenceFactory = std::shared_ptr<AnimationSequence>(*)(
		const YAML::Node&,
		uint16_t,
		std::string,
		uint16_t,
		AssetManager&
	);

	// Factory function for TextureAnimationSequence
	AnimationSequenceFactory textureAnimationSequence = [](
		const YAML::Node& node,
		uint16_t sequenceIndex,
		std::string sequenceId,
		uint16_t framesPerSecond,
		AssetManager& locator
		) -> std::shared_ptr<AnimationSequence>
		{
			auto sequence = std::make_unique<TextureAnimationSequence>(sequenceIndex, sequenceId, framesPerSecond);
			sequence->Deserialize(node, locator);
			return sequence;
		};

	// Map of sequence type names to their factory functions
	std::map<std::string, AnimationSequenceFactory> animationSequenceFactories {
		{"TextureAnimationSequence", textureAnimationSequence}
	};

	// Iterate over nodes in the input YAML
	uint16_t sequenceIndex = 0;
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string sequenceId = (*it)["sequenceId"].as<std::string>();
		uint16_t framesPerSecond = (*it)["framesPerSecond"].as<uint16_t>();
		for (const auto& frames : (*it)["frames"])
		{
			std::string sequenceType = frames.first.as<std::string>();
			std::shared_ptr<AnimationSequence> sequence = animationSequenceFactories[sequenceType](
				frames.second,
				sequenceIndex++,
				sequenceId,
				framesPerSecond,
				assetManager
			);
			AddAnimationSequence(sequence);
		}
	}
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

// ----------------------------------------------------------
void Animation::SortSequencesByIndex(std::vector<SequencePair>& outSortedSequences)
{
	outSortedSequences.assign(mSequences.begin(), mSequences.end());
	std::sort(outSortedSequences.begin(), outSortedSequences.end(), [](const auto& a, const auto& b) {
		return a.second->GetSequenceIndex() < b.second->GetSequenceIndex();
	});
}