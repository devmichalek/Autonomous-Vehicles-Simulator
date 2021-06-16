#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>


class DWallManager
{
	std::vector<sf::RectangleShape> m_rectangleShape;
public:
	DWallManager();
	~DWallManager();

	inline bool intersect(std::array<sf::Vector2f, 4> segments)
	{

	}

	// Load map from file system
	void load(const char* path);
};