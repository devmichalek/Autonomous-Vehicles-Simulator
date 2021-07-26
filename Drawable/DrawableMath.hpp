#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Neural.hpp"

const unsigned WALL_NUMBER_OF_POINTS = 2;
const unsigned CAR_NUMBER_OF_POINTS = 4;
const unsigned CAR_NUMBER_OF_SENSORS = 5;

using Wall = std::array<sf::Vector2f, WALL_NUMBER_OF_POINTS>;
using WallVector = std::vector<Wall>;
using Line = std::array<sf::Vertex, WALL_NUMBER_OF_POINTS>;
using CarPoints = std::array<sf::Vector2f, CAR_NUMBER_OF_POINTS>;
using CarBeams = std::array<Wall, CAR_NUMBER_OF_SENSORS>;
using CarBeamAngles = std::array<double, CAR_NUMBER_OF_SENSORS>;
using CarSensors = std::array<Neuron, CAR_NUMBER_OF_SENSORS>;
class DrawableCar;
using DrawableCarFactory = std::vector<std::pair<DrawableCar*, bool>>;

inline bool Ccw(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
{
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

// Returns true if there is intersection between wall and wall described in two points
inline bool Intersect(Wall& s, sf::Vector2f& x, sf::Vector2f& y)
{
    return Ccw(s[0], x, y) != Ccw(s[1], x, y) && Ccw(s[0], s[1], x) != Ccw(s[0], s[1], y);
}

// Check if wall intersects with car
// This function does not work with collinear points!
inline bool Intersect(Wall& wall, CarPoints& carPoints)
{
    if (Intersect(wall, carPoints[0], carPoints[1]))
        return true;

    if (Intersect(wall, carPoints[1], carPoints[2]))
        return true;

    if (Intersect(wall, carPoints[2], carPoints[3]))
        return true;

    if (Intersect(wall, carPoints[3], carPoints[0]))
        return true;

    return false;
}

// Based on triangle vertices calculate area
inline float GetTriangleArea(sf::Vector2f& A, sf::Vector2f& B, sf::Vector2f& C)
{
	return std::fabs((B.x * A.y - A.x * B.y) + (C.x * B.y - B.x * C.y) + (A.x * C.y - C.x * A.y)) / 2;
}

// Calculates rectangle area by calculating four triangles
inline float GetRectangleArea(CarPoints& carPoints, sf::Vector2f P)
{
    sf::Vector2f& A = carPoints[0];
    sf::Vector2f& B = carPoints[1];
    sf::Vector2f& C = carPoints[2];
    sf::Vector2f& D = carPoints[3];

    float ABP = GetTriangleArea(A, B, P);
    float BCP = GetTriangleArea(B, C, P);
    float CDP = GetTriangleArea(C, D, P);
    float DAP = GetTriangleArea(D, A, P);

    float area = ABP + BCP + CDP + DAP;
    return area;
}

// Calculates midpoint of two points
inline sf::Vector2f GetMidpoint(sf::Vector2f a, sf::Vector2f b)
{
	return sf::Vector2f((a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f);
}

// Checks if there is intersection point between two walls
inline bool GetIntersectionPoint(Wall& a, Wall& b, sf::Vector2f& result)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = a[1].x - a[0].x;
    s1_y = a[1].y - a[0].y;
    s2_x = b[1].x - b[0].x;
    s2_y = b[1].y - b[0].y;

    float s = (-s1_y * (a[0].x - b[0].x) + s1_x * (a[0].y - b[0].y)) / (-s2_x * s1_y + s1_x * s2_y);
    float t = (s2_x * (a[0].y - b[0].y) - s2_y * (a[0].x - b[0].x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        result.x = a[0].x + (t * s1_x);
        result.y = a[0].y + (t * s1_y);
        return true;
    }

    // No collision
    return false;
}

// Calculates length of a wall
inline double GetWallLength(Wall& wall)
{
    return std::sqrt(std::pow(wall[0].x - wall[1].x, 2) + std::pow(wall[0].y - wall[1].y, 2));
}