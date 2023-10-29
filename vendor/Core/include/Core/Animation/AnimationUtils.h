#include <filesystem>

#include <SFML/Graphics.hpp>

// Forward declarations
class Animation;

namespace fs = std::filesystem;

void ExportSpritesheet(const Animation& animation, const fs::path& filePath, uint16_t margin = 0, uint16_t spacing = 0);
void SaveSpritesheetToFile(const fs::path& filePath, uint32_t width, uint32_t height, const std::vector<sf::Sprite> sprites);