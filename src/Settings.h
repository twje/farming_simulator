#include <unordered_map>
#include <string>

#include <SFML/Graphics.hpp>

constexpr char* CAPTION = "Stardew Valley";
constexpr uint16_t WIDTH = 1280;
constexpr uint16_t HEIGHT = 720;
constexpr uint16_t TILESIZE = 64;

const extern std::unordered_map<std::string, sf::Vector2f> OVERLAY_POSITIONS;