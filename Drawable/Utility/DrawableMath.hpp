#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

using Triangle = std::array<sf::Vector2f, 3>;
using TriangleVector = std::vector<Triangle>;
using Line = std::array<sf::Vertex, 2>;
using Edge = std::array<sf::Vector2f, 2>;
using EdgeVector = std::vector<Edge>;

class DrawableMath
{
    inline static bool Ccw(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
    {
        return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
    }

    inline static float TriangleSign(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

public:

    // Based on triangle vertices calculate area
    inline static float GetTriangleArea(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2f& C)
    {
        return std::fabs((B.x * A.y - A.x * B.y) + (C.x * B.y - B.x * C.y) + (A.x * C.y - C.x * A.y)) / 2;
    }

    // Returns true if there is intersection between edge and edge described in two points
    inline static bool Intersect(const Edge& s, const sf::Vector2f& x, const sf::Vector2f& y)
    {
        return Ccw(s[0], x, y) != Ccw(s[1], x, y) && Ccw(s[0], s[1], x) != Ccw(s[0], s[1], y);
    }

    // Returns true if there is intersection between two edges
    inline static bool Intersect(const Edge& a, const Edge& b)
    {
        return Intersect(a, b[0], b[1]);
    }

    inline static bool IntersectNonCollinear(const Edge& a, const Edge& b)
    {
        if (a[0] == b[0])
            return a[1] == b[1];
        if (a[1] == b[0])
            return a[0] == b[1];
        if (a[0] == b[1])
            return a[1] == b[0];
        if (a[1] == b[1])
            return a[0] == b[0];
        return Intersect(a, b[0], b[1]);
    }

    // Checks if edge intersects with any vertex
    // This function does not work with collinear points!
    inline static bool Intersect(const Edge& edge, const sf::VertexArray& vertices)
    {
        size_t lastIndex = vertices.getVertexCount() - 1;
        for (size_t index = 0; index < lastIndex; ++index)
        {
            if (Intersect(edge, vertices[index].position, vertices[index + 1].position))
                return true;
        }

        if (Intersect(edge, vertices[lastIndex].position, vertices[0].position))
            return true;

        return false;
    }

    // Calculates end point
    inline static sf::Vector2f GetEndPoint(sf::Vector2f point, double angle, float length)
    {
        float x = point.x + length * float(cos(angle * M_PI / 180));
        float y = point.y - length * float(sin(angle * M_PI / 180));
        return sf::Vector2f(x, y);
    }

    // Checks if there is intersection point between two edges
    inline static bool GetIntersectionPoint(const Edge& a, const Edge& b, sf::Vector2f& result)
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
    inline static double DifferenceVectorAngle(sf::Vector2f a, sf::Vector2f b)
    {
        auto deltaY = b.y - a.y; // y axis is upside down
        auto deltaX = a.x - b.x;
        return atan2(deltaY, deltaX) * 180.0 / M_PI;
    }

    inline static double CastAtan2ToFullAngle(double angle)
    {
        return angle < 0 ? (angle + 360) : angle;
    }

    // Calculates distance between two points
    inline static double Distance(const sf::Vector2f a, const sf::Vector2f b)
    {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }

    // Calculates length of an edge
    inline static double Distance(const Edge& edge)
    {
        return Distance(edge[0], edge[1]);
    }

    // Returns true if point is inside triangle and false otherwise
    inline static bool IsPointInsideTriangle(const Triangle& triangle, const sf::Vector2f& point)
    {
        float d1 = TriangleSign(point, triangle[0], triangle[1]);
        float d2 = TriangleSign(point, triangle[1], triangle[2]);
        float d3 = TriangleSign(point, triangle[2], triangle[0]);

        bool negative = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool positive = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(negative && positive);
    }

    // Returns true if point is inside circle
    inline static bool IsPointInsideCircle(const sf::Vector2f& center, double radius, const sf::Vector2f& point)
    {
        auto x = std::pow(double(point.x) - center.x, 2.0);
        auto y = std::pow(double(point.y) - center.y, 2.0);
        if (x + y < std::pow(radius, 2.0))
            return true;
        return false;
    }

    // Returns true if point is inside rectangle (rectangle must have angle equal to 0 relative to x axis)
    inline static bool IsPointInsideRectangle(sf::Vector2f rectangleSize, sf::Vector2f rectanglePosition, const sf::Vector2f& point)
    {
        if (point.x < rectanglePosition.x)
            return false;

        if (point.x > rectanglePosition.x + rectangleSize.x)
            return false;

        if (point.y < rectanglePosition.y)
            return false;

        if (point.y > rectanglePosition.y + rectangleSize.y)
            return false;

        return true;
    }

    // Returns true if any vertex is inside triangle and false otherwise
    inline static bool Intersect(const Triangle& triangle, const sf::VertexArray& vertices)
    {
        size_t size = vertices.getVertexCount();
        for (size_t i = 0; i < size; ++i)
        {
            if (IsPointInsideTriangle(triangle, vertices[i].position))
                return true;
        }

        return false;
    }
};
