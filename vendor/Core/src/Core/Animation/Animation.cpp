#include "Core/Animation/Animation.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include "Core/Animation/Sequence/TextureAnimationSequence.h"
#include "Core/Animation/Sequence/SpritesheetAnimationSequence.h"
#include "Core/AssetManager.h"

// ----------------------------------------------------------
Animation::Animation(std::vector<std::unique_ptr<AnimationSequence>>&& sequences)
{
	for (auto& sequence : sequences)
	{
		AddAnimationSequence(std::move(sequence));
	}
}

// ----------------------------------------------------------
/*virtual*/ void Animation::ResolveAssetDeps(AssetManager& assetManager)
{
	for (const auto& sequence : mSequences)
	{
		sequence->ResolveAssetDepsImpl(assetManager);
	}
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

// ------------------------------------------------------------------------
void Animation::SaveToFile(const std::string& filePath)
{
	YAML::Emitter emitter;
	Serialize(emitter);
	std::ofstream file(filePath);
	file << emitter.c_str();
}

// ------------------------------------------------------------------------
std::unique_ptr<Animation> Animation::LoadFromFile(const std::string& filePath)
{
	YAML::Node node = YAML::LoadFile(filePath);
	return Deserialize(node);
}

// ------------------------------------------------------------------------
void Animation::Serialize(YAML::Emitter& emitter)
{
	// Serialize
	emitter << YAML::BeginSeq;
	for (const auto& sequence : mSequences)
	{
		sequence->Serialize(emitter);
	}
	emitter << YAML::EndSeq;
}

// ------------------------------------------------------------------------
std::unique_ptr<Animation> Animation::Deserialize(const YAML::Node& node)
{
	using AnimationSequenceFactory = std::unique_ptr<AnimationSequence>(*)(const YAML::Node&);
	
	std::unordered_map<std::string, AnimationSequenceFactory> animationSequenceFactories
	{
		{"TextureAnimationSequence", CreateAnimationSequenceFactory<TextureAnimationSequence>},
		{"SpritesheetAnimationSequence", CreateAnimationSequenceFactory<SpritesheetAnimationSequence>}
	};
	
	std::vector<std::unique_ptr<AnimationSequence>> sequences;
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string clazz = (*it)["class"].as<std::string>();
		const YAML::Node& state = (*it)["state"];

		auto factory = animationSequenceFactories.find(clazz);
		if (factory != animationSequenceFactories.end())
		{
			auto sequence = factory->second(state);
			sequences.push_back(std::move(sequence));
		}
		else
		{
			throw std::logic_error("Animation sequence type not supported.");
		}
	}

	return std::make_unique<Animation>(std::move(sequences));
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