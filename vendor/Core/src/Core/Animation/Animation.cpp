#include "Core/Animation/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/Animation/AnimationSequence.h"
#include "Core/AssetManager.h"

// ----------------------------------------------------------
Animation::Animation(std::vector<std::unique_ptr<AnimationSequence>>& sequences)
{
	for (auto& sequence : sequences)
	{
		AddAnimationSequence(std::move(sequence));
	}
}

// ----------------------------------------------------------
const AnimationSequence& Animation::GetSequence(const std::string& sequenceId) const
{
	auto iter = mSequenceLookup.find(sequenceId);
	assert(iter != mSequenceLookup.end());
	return *mSequences[iter->second];
}

// ----------------------------------------------------------
void Animation::AddAnimationSequence(std::unique_ptr<AnimationSequence> sequence)
{
	const auto& sequenceId = sequence->GetSequenceId();
	assert(mSequenceLookup.find(sequenceId) == mSequenceLookup.end());

	if (mSequences.size() == 0)
	{
		mStartSequenceId = sequenceId;
	}
	mSequenceLookup[sequenceId] = mSequences.size();
	mSequences.emplace_back(std::move(sequence));
}

// ----------------------------------------------------------
void AnimationFactory::AddAnimationSequenceFactory(std::unique_ptr<AnimationSequenceFactory> factory)
{
	mSequenceFactories.emplace_back(std::move(factory));
}

// ------------------------------------------------------------------------
std::unique_ptr<Animation> AnimationFactory::CreateAnimation(AssetManager& assetManager)
{
	std::vector<std::unique_ptr<AnimationSequence>> sequences;

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
		sequence->Serialize(emitter);	
	}
	emitter << YAML::EndSeq;
}

// ------------------------------------------------------------------------
void AnimationFactory::Deserialize(const YAML::Node& node)
{
	using AnimationSequenceFactoryLoader = std::unique_ptr<AnimationSequenceFactory>(*)(const YAML::Node&);

	// AnimationSequence loaders
	AnimationSequenceFactoryLoader textureAnimationSequenceLoader = [](
		const YAML::Node& node		
		) -> std::unique_ptr<AnimationSequenceFactory>
	{
		auto sequence = std::make_unique<TextureAnimationSequenceFactory>();
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
		std::string clazz = (*it)["class"].as<std::string>();
		const YAML::Node& state = (*it)["state"];

		auto sequence = animationSequenceLoaders[clazz](state);
		AddAnimationSequenceFactory(std::move(sequence));
	}
}