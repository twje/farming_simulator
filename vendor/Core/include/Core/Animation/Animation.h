#pragma once

#include <unordered_map>
#include <filesystem>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Core/AssetManager.h"
#include "Core/ISerializable.h"
#include "Core/Utils.h"

namespace fs = std::filesystem;

// Forward declaration
class AnimationSequence;
class AnimationSequenceFactory;

// --------------------------------------------------------------------------------
class Animation : public Asset, private NonCopyableNonMovableMarker
{
	using SequenceMap = std::unordered_map<std::string, std::unique_ptr<AnimationSequence>>;
	using SequencePair = std::pair<std::string, std::unique_ptr<AnimationSequence>>;

public:
	Animation(std::vector<std::unique_ptr<AnimationSequence>>& sequences);

	// Asset interface
	virtual void ResolveAssetDepsImpl(AssetManager& assetManager) 
	{
		// TODO: implement
	}

	// Getters
	const std::string& GetStartSequenceId() const { return mStartSequenceId; }
	const AnimationSequence& GetSequence(const std::string& sequenceId) const;
	const std::vector<std::unique_ptr<AnimationSequence>>& GetSequences() const { return mSequences; }

private:
	void AddAnimationSequence(std::unique_ptr<AnimationSequence> sequence);

private:
	std::string mStartSequenceId;
	std::vector<std::unique_ptr<AnimationSequence>> mSequences;
	std::map<std::string, int> mSequenceLookup;
};

// --------------------------------------------------------------------------------
class AnimationFactory : public ISerializable
{
public:
	std::unique_ptr<Animation> CreateAnimation(AssetManager& assetManager);

	void AddAnimationSequenceFactory(std::unique_ptr<AnimationSequenceFactory> factory);

	// IO
	void SaveToFile(const std::string& filePath);
	void LoadFromFile(const std::string& filePath);

	// ISerialize interface
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

private:
	std::vector<std::unique_ptr<AnimationSequenceFactory>> mSequenceFactories;
};