#pragma once

#include <vector>

#include "Core/ISerializable.h"
#include "Core/AssetManager.h"

// Forward declaration
class Animation;
class AnimationSequence;

// ------------------------------------------------------------------------
class AnimationSequenceFactory : public ISerializable
{
public:
    AnimationSequenceFactory(std::string_view sequenceId, uint16_t framesPerSecond)
        : mSequenceId(sequenceId),
          mFramesPerSecond(framesPerSecond)
    { }

    // Getters
    const std::string& GetSequenceId() const { return mSequenceId; }
    uint16_t GetFramesPerSecond() const { return mFramesPerSecond; }

    // Hooks
    virtual std::unique_ptr<Animation> CreateAnimationSequence() = 0;

public:
    std::string mSequenceId;
    uint16_t mFramesPerSecond;
};

// ------------------------------------------------------------------------
class AnimationFactory : public ISerializable
{
public:
    std::unique_ptr<Animation> CreateAnimation(AssetManager& assetManager);

    void AddAnimationSequenceFactory(std::unique_ptr<AnimationSequenceFactory> factory)
    {
        mSequenceFactories.emplace_back(std::move(factory));
    }

    // IO
    void SaveToFile(const std::string& filePath);
    void LoadFromFile(const std::string& filePath);

    // ISerialize interface
    void Serialize(YAML::Emitter& emitter) override;
    void Deserialize(const YAML::Node& node, AssetManager& assetManager) override;

private:
    std::vector<std::unique_ptr<AnimationSequenceFactory>> mSequenceFactories;
};