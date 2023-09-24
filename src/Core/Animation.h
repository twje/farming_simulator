#pragma once

#include <map>
#include <cassert>
#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "Core/GameObject.h"
#include "Core/Timer.h"
#include "Core/ISerializable.h"

// Forward declaration
class TextureManager;

struct SequenceFrame
{	
	sf::Texture* mTexture{ nullptr };
	sf::IntRect mTextureRect;

	sf::Vector2f GetOrigin(const sf::Vector2f& originAnchor)
	{
		float originX = originAnchor.x * mTextureRect.getSize().x;
		float originY = originAnchor.y * mTextureRect.getSize().y;
		return sf::Vector2f(originX, originY);
	}
};

class AnimationSequence : public ISerializable
{
public:
	AnimationSequence(std::string sequenceId, uint16_t framesPerSecond)
		: mSequenceId(sequenceId),		
		  mFramesPerSecond(framesPerSecond),
		  mDuration(1.0f / framesPerSecond)
	{ }
	
	// Getters
	const std::string& GetSequenceId() { return mSequenceId; }	
	float GetDuration() { return mDuration; }
	uint16_t GetFramesPerSecond() const { return mFramesPerSecond; }
	
	// Hooks
	virtual void GetFrame(SequenceFrame& outFrame, uint16_t frameIndex) = 0;
	virtual uint16_t GetFrameCount() = 0;

private:
	std::string mSequenceId;	
	float mDuration;
	uint16_t mFramesPerSecond;
};

class Animation : public ISerializable
{
public:	
	Animation() = default;

	Animation(const Animation& other)		
	{
		for (const auto& sequencePair : other.mSequences)
		{
			AddAnimationSequence(sequencePair.second);
		}
	}

	void Upate(const sf::Time& timestamp)
	{
		assert(mCurrentSequence != nullptr);
		mTimer.Update(timestamp);
		if (mTimer.IsFinished())
		{
			mFrameIndex = (mFrameIndex + 1) % mCurrentSequence->GetFrameCount();	
			UpdateFrame();
			mTimer.Reset();
		}
	}

	// Getters
	sf::Sprite& GetSprite() { return *mSprite; }

	sf::FloatRect GetGlobalBounds() const
	{ 
		assert(mSprite != nullptr);
		return mSprite->getGlobalBounds(); 
	}

	// Setters
	void SetOriginAnchor(sf::Vector2f originAnchor) 
	{ 
		mOriginAnchor = originAnchor;
		auto& frame = GetFrame();
		mSprite->setOrigin(frame.GetOrigin(mOriginAnchor));
	}

	void AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence)
	{		
		const auto& sequenceId = sequence->GetSequenceId();
		assert(mSequences.find(sequenceId) == mSequences.end());
		
		mSequences[sequenceId] = sequence;
		if (mCurrentSequence == nullptr)
		{
			Initialize(sequenceId);
		}
	}

	void SetAnimationSequence(const std::string& sequenceId)
	{
		auto iter = mSequences.find(sequenceId);
		assert(iter != mSequences.end());
		mCurrentSequence = iter->second.get();
		mTimer.Reset(mCurrentSequence->GetDuration());
		mFrameIndex = 0;
	}

	void SaveToFile(const std::string& filePath)
	{
		YAML::Emitter emitter;
		Serialize(emitter);
		std::ofstream file(filePath);
		file << emitter.c_str();
	}

	void LoadFromFile(const std::string& filePath, ResourceLocator& locator)
	{
		YAML::Node node = YAML::LoadFile(filePath);
		Deserialize(node, locator);
	}

	// ISerialize interface
	void Serialize(YAML::Emitter& emitter) override
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

	void Deserialize(const YAML::Node& node, ResourceLocator& locator) override;

private:
	SequenceFrame& GetFrame()
	{
		assert(mCurrentSequence != nullptr);
		mCurrentSequence->GetFrame(mOutSequenceFrame, mFrameIndex);
		return mOutSequenceFrame;
	}

	void Initialize(std::string sequenceId)
	{
		SetAnimationSequence(sequenceId);
		auto& frame = GetFrame();
		mSprite = std::make_unique<sf::Sprite>(*frame.mTexture);
		mSprite->setOrigin(frame.GetOrigin(mOriginAnchor));
	}

	void UpdateFrame()
	{
		auto& frame = GetFrame();
		mSprite->setTexture(*frame.mTexture);
		mSprite->setTextureRect(frame.mTextureRect);
		mSprite->setOrigin(frame.GetOrigin(mOriginAnchor));
	}

private:
	// sequence memebers
	std::map<std::string, std::shared_ptr<AnimationSequence>> mSequences;
	AnimationSequence* mCurrentSequence{ nullptr };
	SequenceFrame mOutSequenceFrame;	

	// animation members
	uint16_t mFrameIndex{ 0 };
	Timer mTimer;
	
	// graphics members
	sf::Vector2f mOriginAnchor;
	std::unique_ptr<sf::Sprite> mSprite;	
};