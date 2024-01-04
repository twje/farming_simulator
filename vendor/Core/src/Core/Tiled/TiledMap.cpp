#include "Core/Tiled/TiledMap.h"

sf::IntRect ConvertTsonRectToSFMLIntRect(const tson::Rect& rect)
{
	return { sf::Vector2i(rect.x, rect.y), sf::Vector2i(rect.width, rect.height) };
}