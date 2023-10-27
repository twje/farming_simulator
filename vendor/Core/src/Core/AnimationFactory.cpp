#include "Core/AnimationFactory.h"
#include "Core/Animation.h"

// ------------------------------------------------------------------------
std::unique_ptr<Animation> AnimationFactory::CreateAnimation(AssetManager& assetManager)
{
	return nullptr;
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
void AnimationFactory::Deserialize(const YAML::Node& node, AssetManager& assetManager)
{

}