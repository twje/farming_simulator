#pragma once

#include <string>
#include <initializer_list>

#include "Core/Animation.h"
#include "Core/TextureManager.h"

class TextureAnimationSequence : public AnimationSequence
{
public:
	TextureAnimationSequence(
		std::string sequenceId, 
		uint16_t framesPerSecond, 
		TextureManager& textureManager,
		std::initializer_list<std::string> frames
	)
		: AnimationSequence(sequenceId, framesPerSecond),
		  mTextureManager(textureManager)
	{
		for (const std::string& textureId : frames)
		{
			AddFrame(textureId);
		}
	}

	void GetFrame(SequenceFrame& outFrame, uint16_t frameIndex) override
	{
		sf::Texture* texture = mFrames.at(frameIndex);
		if (texture != outFrame.mTexture)
		{
			outFrame.mTexture = texture;
			outFrame.mTextureRect = sf::IntRect(sf::Vector2i(), sf::Vector2i(outFrame.mTexture->getSize()));
		}
	}

	virtual uint16_t GetFrameCount() { return mFrames.size(); }

private:
	void TextureAnimationSequence::AddFrame(std::string textureId)
	{			
		mFrames.emplace_back(&mTextureManager.Get(textureId));
	}

private:
	TextureManager& mTextureManager;
	std::vector<sf::Texture*> mFrames;
};