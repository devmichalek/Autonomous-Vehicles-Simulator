#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>

using Segment = std::array<sf::Vector2f, 2>;
using SegmentVector = std::vector<Segment>;
using Line = std::array<sf::Vertex, 2>;
using CarPoints = std::array<sf::Vector2f, 4>;
using CarBeams = std::array<Segment, 5>;

inline bool ccw(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
{
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

inline bool intersect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d)
{
	return ccw(a, c, d) != ccw(b, c, d) && ccw(a, b, c) != ccw(a, b, d);
}

inline float calculateTriangleArea(sf::Vector2f& A, sf::Vector2f& B, sf::Vector2f& C)
{
	return std::fabs((B.x * A.y - A.x * B.y) + (C.x * B.y - B.x * C.y) + (A.x * C.y - C.x * A.y)) / 2;
}

inline sf::Vector2f midpoint(sf::Vector2f a, sf::Vector2f b)
{
	return sf::Vector2f((a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f);
}

inline bool intersect(Segment& a, Segment& b, sf::Vector2f& ipoint)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = a[1].x - a[0].x;
    s1_y = a[1].y - a[0].y;
    s2_x = b[1].x - b[0].x;
    s2_y = b[1].y - b[0].y;

    float s, t;
    s = (-s1_y * (a[0].x - b[0].x) + s1_x * (a[0].y - b[0].y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (a[0].y - b[0].y) - s2_y * (a[0].x - b[0].x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        ipoint.x = a[0].x + (t * s1_x);
        ipoint.y = a[0].y + (t * s1_y);
        return true;
    }

    // No collision
    return false;
}