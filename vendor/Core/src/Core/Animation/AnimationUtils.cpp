#include <yaml-cpp/yaml.h>

#include "Core/Animation/AnimationUtils.h"
#include "Core/Animation/Animation.h"
#include "Core/Animation/AnimationSequence.h"
#include "Core/Animation/Sequence/SpritesheetAnimationSequence.h"
#include "Core/TextureRegion.h"
#include "Core/StringUtils.h"

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::Export(const Animation& animation, const std::string name, const fs::path& outputDirectory,
	                                      uint16_t margin, uint16_t spacing)
{
	// Prepare state
	State state( margin, spacing);
	SetFilePathsExportState(name, outputDirectory, state);
	SetGraphicsExportState(animation, state);

	// Export
	ExportSpritesheetConfig(state);
	ExportSpritesheetTexture(state);
	ExportAnimationConfig(animation, state);
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::SetFilePathsExportState(const std::string name, const fs::path& outputDirectory, State& outState)
{
	outState.mTextureId = "sprt" + CapitalizeString(name) + "Tex";
	outState.mSpritesheetId = "sprt" + CapitalizeString(name);
	outState.mSpritesheetConfigFilePath = outputDirectory / (outState.mSpritesheetId + ".yaml");
	outState.mSpritesheetTextureFilePath = outputDirectory / (outState.mTextureId + ".png");
	outState.mAnimationFilePath = outputDirectory / ("anim" + CapitalizeString(name) + ".yaml");
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::SetGraphicsExportState(const Animation& animation, State& outState)
{
	TextureRegion textureRegion;
	const auto& sequences = animation.GetSequences();

	outState.mRows = sequences.size();
	for (const auto& sequence : sequences)
	{
		uint32_t sequenceWidth = 0, sequenceHeight = 0;

		outState.mCols = std::max(outState.mCols, sequence->GetFrameCount());
		for (uint32_t frameIndex = 0; frameIndex < sequence->GetFrameCount(); ++frameIndex)
		{
			sequence->GetFrame(textureRegion, frameIndex);
			sf::IntRect region = textureRegion.GetRegion();

			sf::Sprite sprite(*textureRegion.GetTexture(), region);
			sprite.setPosition(sf::Vector2f(sequenceWidth, outState.mTextureHeight));
			outState.mSprites.emplace_back(std::move(sprite));

			sequenceWidth += region.width;
			sequenceHeight = std::max(sequenceHeight, static_cast<uint32_t>(region.height));
		}

		outState.mTextureWidth = std::max(outState.mTextureWidth, sequenceWidth);
		outState.mTextureHeight += sequenceHeight;
	}
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::ExportSpritesheetConfig(const State& state)
{
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "textureId" << YAML::Value << state.mTextureId;
	emitter << YAML::Key << "rows" << YAML::Value << state.mRows;
	emitter << YAML::Key << "cols" << YAML::Value << state.mCols;
	emitter << YAML::Key << "margin" << YAML::Value << state.mMargin;
	emitter << YAML::Key << "spacing" << YAML::Value << state.mSpacing;
	emitter << YAML::EndMap;

	std::ofstream file(state.mSpritesheetConfigFilePath);
	file << emitter.c_str();
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::ExportSpritesheetTexture(const State& state)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(sf::Vector2u(state.mTextureWidth, state.mTextureHeight)))
	{
		throw std::runtime_error("Unable to create to render texture");
	}

	renderTexture.clear();
	for (const sf::Sprite& sprite : state.mSprites)
	{
		renderTexture.draw(sprite);
	}
	renderTexture.display();

	const sf::Texture& texture = renderTexture.getTexture();
	sf::Image image = texture.copyToImage();
	if (!image.saveToFile(state.mSpritesheetTextureFilePath))
	{
		throw std::runtime_error("Failed to save image to file");
	}
}

// --------------------------------------------------------------------------------
void SpritesheetAnimationExporter::ExportAnimationConfig(const Animation& animation, const State& state)
{
	AnimationFactory factory;

	for (size_t row = 0; row < animation.GetSequences().size(); ++row)
	{
		const auto& sequence = animation.GetSequences().at(row);

		std::vector<uint16_t> frames;
		for (size_t col = 0; col < sequence->GetFrameCount(); ++col)
		{
			frames.emplace_back(row * state.mCols + col);
		}

		auto sequenceFactory = std::make_unique<SpritesheetAnimationSequenceFactory>(
			sequence->GetSequenceId(),
			sequence->GetFramesPerSecond(),
			state.mSpritesheetId,
			frames
		);
		factory.AddAnimationSequenceFactory(std::move(sequenceFactory));
	}
	factory.SaveToFile(state.mAnimationFilePath.string());
}