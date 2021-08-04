#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

const size_t EDGE_NUMBER_OF_POINTS = 2;
const size_t TRIANGLE_NUMBER_OF_POINTS = 3;
const size_t CAR_NUMBER_OF_POINTS = 4;
const size_t CAR_NUMBER_OF_SENSORS = 5;
const size_t CAR_NUMBER_OF_INPUTS = 3;

using Edge = std::array<sf::Vector2f, EDGE_NUMBER_OF_POINTS>;
using EdgeVector = std::vector<Edge>;
using Triangle = std::array<sf::Vector2f, TRIANGLE_NUMBER_OF_POINTS>;
using Line = std::array<sf::Vertex, EDGE_NUMBER_OF_POINTS>;
using CarPoints = std::array<sf::Vector2f, CAR_NUMBER_OF_POINTS>;
using CarBeams = std::array<Edge, CAR_NUMBER_OF_SENSORS>;
using CarBeamAngles = std::array<double, CAR_NUMBER_OF_SENSORS>;
class DrawableCar;
using DetailedCar = std::pair<DrawableCar*, bool>;
using DetailedCarFactory = std::vector<DetailedCar>;

inline bool Ccw(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
{
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

// Returns true if there is intersection between edge and edge described in two points
inline bool Intersect(const Edge& s, const sf::Vector2f& x, const sf::Vector2f& y)
{
    return Ccw(s[0], x, y) != Ccw(s[1], x, y) && Ccw(s[0], s[1], x) != Ccw(s[0], s[1], y);
}

// Returns true if there is intersection between two edges
inline bool Intersect(const Edge& a, const Edge& b)
{
    return Intersect(a, b[0], b[1]);
}

// Check if edge intersects with car
// This function does not work with collinear points!
inline bool Intersect(const Edge& edge, const CarPoints& carPoints)
{
    if (Intersect(edge, carPoints[0], carPoints[1]))
        return true;

    if (Intersect(edge, carPoints[1], carPoints[2]))
        return true;

    if (Intersect(edge, carPoints[2], carPoints[3]))
        return true;

    if (Intersect(edge, carPoints[3], carPoints[0]))
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

// Calculates end point
inline sf::Vector2f GetEndPoint(sf::Vector2f point, double angle, float length)
{
    float x = point.x + length * float(cos(angle * M_PI / 180));
    float y = point.y - length * float(sin(angle * M_PI / 180));
    return sf::Vector2f(x, y);
}

// Checks if there is intersection point between two edges
inline bool GetIntersectionPoint(const Edge& a, const Edge& b, sf::Vector2f& result)
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

// Calculate angle between the line defined by two points and the horizontal axis
inline double DifferenceVectorAngle(sf::Vector2f a, sf::Vector2f b)
{
    auto deltaY = b.y - a.y; // y axis is upside down
    auto deltaX = a.x - b.x;
    return atan2(deltaY, deltaX) * 180.0 / M_PI;
}

inline double CastAtan2ToFullAngle(double angle)
{
    return angle < 0 ? (angle + 360) : angle;
}

// Calculates distance between two points
inline double Distance(const sf::Vector2f a, const sf::Vector2f b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

// Calculates length of a edge
inline double Distance(const Edge& edge)
{
    return Distance(edge[0], edge[1]);
}

inline float TriangleSign(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

// Check if point is inside triangle
inline bool IsPointInsideTriangle(const Triangle& triangle, const sf::Vector2f point)
{
    float d1 = TriangleSign(point, triangle[0], triangle[1]);
    float d2 = TriangleSign(point, triangle[1], triangle[2]);
    float d3 = TriangleSign(point, triangle[2], triangle[0]);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

// Check if car is inside triangle
inline bool IsCarInsideTriangle(const Triangle& triangle, const CarPoints& car)
{
    for (size_t i = 0; i < CAR_NUMBER_OF_POINTS; ++i)
    {
        if (IsPointInsideTriangle(triangle, car[i]))
            return true;
    }
    
    return false;
}