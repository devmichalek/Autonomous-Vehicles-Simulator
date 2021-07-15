#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>

using Segment = std::array<sf::Vector2f, 2>;
using SegmentVector = std::vector<Segment>;
using Line = std::array<sf::Vertex, 2>;

inline bool ccw(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
{
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

inline bool intersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d)
{
	return ccw(a, c, d) != ccw(b, c, d) && ccw(a, b, c) != ccw(a, b, d);
}
