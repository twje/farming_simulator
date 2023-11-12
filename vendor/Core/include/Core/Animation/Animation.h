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
	void ResolveAssetDepsImpl(AssetManager& assetManager) override;

	// Getters
	const std::string& GetStartSequenceId() const;
	const AnimationSequence& GetSequence(const std::string& sequenceId) const;
	const std::vector<std::unique_ptr<AnimationSequence>>& GetSequences() const;

private:
	void AddAnimationSequence(std::unique_ptr<AnimationSequence> sequence);

private:
	std::string mStartSequenceId;
	std::vector<std::unique_ptr<AnimationSequence>> mSequences;
	std::map<std::string, int> mSequenceLookup;
};

// --------------------------------------------------------------------------------
class AnimationFactory
{
public:
	using SequenceFactoryList = std::vector<std::unique_ptr<AnimationSequenceFactory>>;

	AnimationFactory(SequenceFactoryList&& sequenceFactories);

	std::unique_ptr<Animation> CreateAnimation(AssetManager& assetManager);	

	// IO
	void SaveToFile(const std::string& filePath);
	static std::unique_ptr<AnimationFactory> LoadFromFile(const std::string& filePath);

	// Serializable methods
	void Serialize(YAML::Emitter& emitter);
	static std::unique_ptr<AnimationFactory> Deserialize(const YAML::Node& node);

private:
	SequenceFactoryList mSequenceFactories;
};