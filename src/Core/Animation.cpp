#include "Core/Animation.h"
#include "Core/TextureAnimationSequence.h"
#include "Core/ResourceLocator.h"

void Animation::Deserialize(const YAML::Node& node, ResourceLocator& locator)
{
	// Define a type alias for AnimationSequence factory functions
	using AnimationSequenceFactory = std::shared_ptr<AnimationSequence>(*)(
		const YAML::Node&, 
		std::string, 
		uint16_t,
		ResourceLocator&
	);
	
	// Factory function for TextureAnimationSequence
	AnimationSequenceFactory textureAnimationSequence = [](
		const YAML::Node& node,
		std::string sequenceId,
		uint16_t framesPerSecond,
		ResourceLocator& locator
		) -> std::shared_ptr<AnimationSequence>
		{
			auto sequence = std::make_unique<TextureAnimationSequence>(sequenceId, framesPerSecond);
			sequence->Deserialize(node, locator);
			return sequence;
		};
	
	// Map of sequence type names to their factory functions
	std::map<std::string, AnimationSequenceFactory> animationSequenceFactories {
		{"TextureAnimationSequence", textureAnimationSequence}
	};

	// Iterate over nodes in the input YAML
	for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
	{
		std::string sequenceId = (*it)["key"].as<std::string>();
		uint16_t framesPerSecond = (*it)["framesPerSecond"].as<uint16_t>();
		for (const auto& frames : (*it)["frames"])
		{
			std::string sequenceType = frames.first.as<std::string>();
			std::shared_ptr<AnimationSequence> sequence = animationSequenceFactories[sequenceType](
				frames.second,
				sequenceId,
				framesPerSecond,
				locator
			);
			AddAnimationSequence(sequence);
		}
	}
}