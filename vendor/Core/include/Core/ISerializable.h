#pragma once

#include <yaml-cpp/yaml.h>

// Forward declaration
class ResourceLocator;

class ISerializable
{
public:
    virtual void Serialize(YAML::Emitter& emitter) = 0;
    virtual void Deserialize(const YAML::Node& node, ResourceLocator& locator) = 0;
};