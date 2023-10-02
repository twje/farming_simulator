#include "Settings.h"

const std::unordered_map<std::string, sf::Vector2f> OVERLAY_POSITIONS
{
	{ "tool", { 40, HEIGHT - 15 } },
	{ "seed", { 70, HEIGHT - 5 } }
};

const std::unordered_map<std::string, uint16_t> LAYERS
{
    {"water", 0},
    {"ground", 1},
    {"soil", 2},
    {"soil water", 3},
    {"rain floor", 4},
    {"house bottom", 5},
    {"ground plant", 6},
    {"main", 7},
    {"house top", 8},
    {"fruit", 9},
    {"rain drops", 10}
};