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
void Animation::ExportSpritesheet(const fs::path& filePath, uint16_t margin, uint16_t spacing)
{
	// TODO: implement logic to support margin and spacing
	TextureRegion textureRegion;
	std::vector<sf::Sprite> sprites;

	std::vector<SequencePair> sortedSequences;
	SortSequencesByIndex(sortedSequences);

	uint32_t textureWidth = 0, textureHeight = 0;

	for (const auto& sequencePair : sortedSequences) {
		AnimationSequence& sequence = *sequencePair.second;
		uint32_t sequenceWidth = 0, sequenceHeight = 0;

		for (uint32_t frameIndex = 0; frameIndex < sequence.GetFrameCount(); ++frameIndex) {
			sequence.GetFrame(textureRegion, frameIndex);
			sf::IntRect region = textureRegion.GetRegion();

			sf::Sprite sprite(*textureRegion.GetTexture(), region);
			sprite.setPosition(sf::Vector2f(sequenceWidth, textureHeight));
			sprites.emplace_back(std::move(sprite));

			sequenceWidth += region.width;
			sequenceHeight = std::max(sequenceHeight, static_cast<uint32_t>(region.height));
		}

		textureWidth = std::max(textureWidth, sequenceWidth);
		textureHeight += sequenceHeight;
	}
	SaveSpritesheetToFile(filePath, textureWidth, textureHeight, sprites);
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

// ----------------------------------------------------------
void Animation::SaveSpritesheetToFile(const fs::path& filePath, uint32_t width, uint32_t height, const std::vector<sf::Sprite> sprites)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(sf::Vector2u(width, height)))
	{
		throw std::runtime_error("Unable to create to render texture");
	}

	renderTexture.clear();
	for (const sf::Sprite& sprite : sprites)
	{
		renderTexture.draw(sprite);
	}
	renderTexture.display();

	const sf::Texture& texture = renderTexture.getTexture();
	sf::Image image = texture.copyToImage();
	if (!image.saveToFile(filePath))
	{
		throw std::runtime_error("Failed to save image to file");
	}
}