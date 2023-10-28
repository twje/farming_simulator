#include "Core/AnimationUtils.h"
#include "Core/Animation.h"

void ExportSpritesheet(const Animation& animation, const fs::path& filePath, uint16_t margin, uint16_t spacing)
{
	// TODO: implement logic to support margin and spacing
	TextureRegion textureRegion;
	std::vector<sf::Sprite> sprites;

	uint32_t textureWidth = 0, textureHeight = 0;

	for (const auto& sequencePair : animation.GetAnimationSequences())
	{
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

void SaveSpritesheetToFile(const fs::path& filePath, uint32_t width, uint32_t height, const std::vector<sf::Sprite> sprites)
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