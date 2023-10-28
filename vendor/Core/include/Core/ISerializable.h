#pragma once

#include <yaml-cpp/yaml.h>

// Forward declaration
class AssetManager;

class ISerializable
{
public:
    virtual void Serialize(YAML::Emitter& emitter) = 0;
    // Remove when all resource factories are implemented
    virtual void Deserialize(const YAML::Node& node, AssetManager& assetManager) { };
    virtual void Deserialize(const YAML::Node& node) { };
};