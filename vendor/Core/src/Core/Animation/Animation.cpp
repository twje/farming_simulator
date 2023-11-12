#include "Core/Animation/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Sequence/TextureAnimationSequence.h"
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
/*virtual*/ void Animation::ResolveAssetDepsImpl(AssetManager& assetManager)
{
	// TODO: implement
}

// ----------------------------------------------------------
const std::string& Animation::GetStartSequenceId() const 
{ 
	return mStartSequenceId; 
}

// ----------------------------------------------------------
const AnimationSequence& Animation::GetSequence(const std::string& sequenceId) const
{
	auto iter = mSequenceLookup.find(sequenceId);
	assert(iter != mSequenceLookup.end());
	return *mSequences[iter->second];
}

// ----------------------------------------------------------
const std::vector<std::unique_ptr<AnimationSequence>>& Animation::GetSequences() const
{ 
	return mSequences; 
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

// ------------------------------------------------------------------------
AnimationFactory::AnimationFactory(SequenceFactoryList&& sequenceFactories)
	: mSequenceFactories(std::move(sequenceFactories))
{ }

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
std::unique_ptr<AnimationFactory> AnimationFactory::LoadFromFile(const std::string& filePath)
{
	YAML::Node node = YAML::LoadFile(filePath);
	return Deserialize(node);
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
std::unique_ptr<AnimationFactory> AnimationFactory::Deserialize(const YAML::Node& node)
{
	using AnimationSequenceFactoryLoader = std::unique_ptr<AnimationSequenceFactory>(*)(const YAML::Node&);
	
	// AnimationSequence loaders
	AnimationSequenceFactoryLoader textureAnimationSequenceLoader = [](
		const YAML::Node& node		
		) -> std::unique_ptr<AnimationSequenceFactory>
	{
		return TextureAnimationSequenceFactory::Deserialize(node);
	};

	// AnimationSequence loader lookup
	std::unordered_map<std::string, AnimationSequenceFactoryLoader> animationSequenceLoaders{
		{"TextureAnimationSequence", textureAnimationSequenceLoader}
	};

	// Load all sequence factories
	SequenceFactoryList sequences;
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string clazz = (*it)["class"].as<std::string>();
		const YAML::Node& state = (*it)["state"];

		auto sequence = animationSequenceLoaders[clazz](state);
		sequences.push_back(std::move(sequence));		
	}
	
	return std::make_unique<AnimationFactory>(std::move(sequences));
}