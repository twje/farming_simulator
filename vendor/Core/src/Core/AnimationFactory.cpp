#include "Core/AnimationFactory.h"
#include "Core/Animation.h"
#include "Core/TextureAnimationSequence.h"

// ------------------------------------------------------------------------
std::unique_ptr<Animation> AnimationFactory::CreateAnimation(AssetManager& assetManager)
{
	auto animation = std::make_unique<Animation>();
	for (const auto& factory : mSequenceFactories)
	{
		animation->AddAnimationSequence(std::move(factory->CreateAnimationSequence(assetManager)));
	}
	return animation;
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
	std::unordered_map<std::string, AnimationSequenceFactoryLoader> animationSequenceLoaders {
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