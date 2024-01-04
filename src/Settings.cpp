#include "Settings.h"

const std::unordered_map<std::string, sf::Vector2f> OVERLAY_POSITIONS
{
	{ "tool", { 40, HEIGHT - 15 } },
	{ "seed", { 70, HEIGHT - 5 } }
};

const std::unordered_map<std::string, uint16_t> LAYERS
{
	{ "water", 0},
	{ "ground", 1 },
	{ "soil", 2 },
	{ "soil water", 3 },
	{ "rain floor", 4 },
	{ "house bottom", 5 },
	{ "ground plant", 6 },
	{ "main", 7 },
	{ "house top", 8 },
	{ "fruit", 9 },
	{ "rain drops", 10 }
};

const std::unordered_map<std::string, sf::Vector2f> PLAYER_TOOL_OFFSET  
{
	{ "left", sf::Vector2f(-50, 40) },
	{ "right", sf::Vector2f(50, 40) },
	{ "up", sf::Vector2f(0, -10) },
	{ "down", sf::Vector2f(0, 50) }
};

const std::unordered_map<std::string, std::vector<sf::Vector2f>> APPLE_POSITIONS
{
	{ "Small", { {18.0f, 17.0f}, { 30.0f, 37.0f }, { 12.0f, 50.0f }, { 30.0f, 45.0f }, { 20.0f, 30.0f }, { 30.0f, 10.0f } } },
	{ "Large", { {30.0f, 24.0f}, { 60.0f, 65.0f }, { 50.0f, 50.0f }, { 16.0f, 40.0f }, { 45.0f, 50.0f }, { 42.0f, 70.0f } } }
};