#include "Core/Animation/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/Animation/AnimationSequence.h"
#include "Core/AssetManager.h"


// ----------------------------------------------------------
Animation::Animation(const std::vector<std::shared_ptr<AnimationSequence>>& sequences)
{
	for (const auto& sequence : sequences)
	{
		AddAnimationSequence(sequence);
	}
}

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
void AnimationFactory::AddAnimationSequenceFactory(std::unique_ptr<AnimationSequenceFactory> factory)
{
	mSequenceFactories.emplace_back(std::move(factory));
}

// ------------------------------------------------------------------------
std::unique_ptr<Animation> AnimationFactory::CreateAnimation(AssetManager& assetManager)
{
	std::vector<std::shared_ptr<AnimationSequence>> sequences;

	for (const auto& factory : mSequenceFactories)
	{
		sequences.emplace_back(std::move(factory->CreateAnimationSequence(assetManager)));
	}

	return std::make_unique<Animation>(sequences);
}

// ------------------------------------------------------------------------
void AnimationFactory::SaveToFile(const std::string& filePath)
{
	YAML::Emitter emitter;
	Serialize(emitter);
	std::ofstream file(filePath);
	file << emitter.c_str();
}

// ------------------------------------------------------------------------
void AnimationFactory::LoadFromFile(const std::string& filePath)
{
	YAML::Node node = YAML::LoadFile(filePath);
	Deserialize(node);
}

// ------------------------------------------------------------------------
void AnimationFactory::Serialize(YAML::Emitter& emitter)
{
	// Serialize
	emitter << YAML::BeginSeq;
	for (const auto& sequence : mSequenceFactories)
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "sequenceId" << YAML::Value << sequence->GetSequenceId();
		emitter << YAML::Key << "framesPerSecond" << YAML::Value << sequence->GetFramesPerSecond();
		emitter << YAML::Key << "frames" << YAML::Value;
		sequence->Serialize(emitter);
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;
}

// ------------------------------------------------------------------------
void AnimationFactory::Deserialize(const YAML::Node& node)
{
	// AnimationSequence loader type alias
	using AnimationSequenceFactoryLoader = std::unique_ptr<AnimationSequenceFactory>(*)(
		const YAML::Node&,
		std::string,
		uint16_t
		);

	// AnimationSequence loaders
	AnimationSequenceFactoryLoader textureAnimationSequenceLoader = [](
		const YAML::Node& node,
		std::string sequenceId,
		uint16_t framesPerSecond
		) -> std::unique_ptr<AnimationSequenceFactory>
		{
			auto sequence = std::make_unique<TextureAnimationSequenceFactory>(sequenceId, framesPerSecond);
			sequence->Deserialize(node);
			return sequence;
		};

	// AnimationSequence loader lookup
	std::unordered_map<std::string, AnimationSequenceFactoryLoader> animationSequenceLoaders{
		{"TextureAnimationSequence", textureAnimationSequenceLoader}
	};

	// Load all sequence factories
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string sequenceId = (*it)["sequenceId"].as<std::string>();
		uint16_t framesPerSecond = (*it)["framesPerSecond"].as<uint16_t>();
		for (const auto& frames : (*it)["frames"])
		{
			std::string sequenceType = frames.first.as<std::string>();
			std::unique_ptr<AnimationSequenceFactory> factory = animationSequenceLoaders[sequenceType](
				frames.second,
				sequenceId,
				framesPerSecond
				);
			AddAnimationSequenceFactory(std::move(factory));
		}
	}
}