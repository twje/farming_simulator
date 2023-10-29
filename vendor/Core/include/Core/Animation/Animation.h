#pragma once

#include <unordered_map>
#include <filesystem>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Core/ISerializable.h"

namespace fs = std::filesystem;

// Forward declaration
class AnimationSequence;
class AnimationSequenceFactory;

// --------------------------------------------------------------------------------
class Animation
{
	using SequenceMap = std::unordered_map<std::string, std::shared_ptr<AnimationSequence>>;
	using SequencePair = std::pair<std::string, std::shared_ptr<AnimationSequence>>;

public:
	Animation(const std::vector<std::shared_ptr<AnimationSequence>>& sequences);

	// Getters
	const std::string& GetStartSequenceId() const { return mStartSequenceId; }
	const AnimationSequence& GetSequence(const std::string& sequenceId) const;
	const SequenceMap& GetSequences() const { return mSequences; }

private:
	void AddAnimationSequence(std::shared_ptr<AnimationSequence> sequence);

private:
	std::string mStartSequenceId;
	SequenceMap mSequences;
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