#pragma once
#include <vector>
#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>
#include <Box2D\b2_math.h>

using Rectangle = std::array<sf::Vector2f, 4>;
using RectangleVector = std::vector<Rectangle>;
using RectangleShape = std::array<sf::Vertex, 4>;
using Edge = std::array<sf::Vector2f, 2>;
using EdgeVector = std::vector<Edge>;
using EdgeShape = std::array<sf::Vertex, 2>;
using TriangleShape = std::array<sf::Vertex, 3>;

class MathContext
{
    // Returns true if three points are making a clockwise order
    inline static bool Clockwise(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
    {
        const float x1 = (b.x - a.x);
        const float y1 = (b.y - a.y);
        const float x2 = (c.x - a.x);
        const float y2 = (c.y - a.y);
        return y2 * x1 > y1 * x2;
    }

    // Calculates cross product
    inline static int CrossProduct(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
    {
        const float x1 = (b.x - a.x);
        const float y1 = (b.y - a.y);
        const float x2 = (c.x - a.x);
        const float y2 = (c.y - a.y);
        return int(x1 * y2 - y1 * x2);
    }

    inline static float SFML_BOX2D_SCALE = 40.0f;

public:

    // Returns true if there is intersection between edge and edge described in two points
    inline static bool Intersect(const Edge& s, const sf::Vector2f& x, const sf::Vector2f& y)
    {
        return Clockwise(s[0], x, y) != Clockwise(s[1], x, y) && Clockwise(s[0], s[1], x) != Clockwise(s[0], s[1], y);
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

    // Calculates end point
    inline static sf::Vector2f GetEndPoint(sf::Vector2f point, double angle, float length)
    {
        const float x = point.x + length * float(cos(angle * M_PI / 180));
        const float y = point.y - length * float(sin(angle * M_PI / 180));
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

        const float s = (-s1_y * (a[0].x - b[0].x) + s1_x * (a[0].y - b[0].y)) / (-s2_x * s1_y + s1_x * s2_y);
        const float t = (s2_x * (a[0].y - b[0].y) - s2_y * (a[0].x - b[0].x)) / (-s2_x * s1_y + s1_x * s2_y);

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

    // Calculates distance between two points
    inline static double Distance(const sf::Vector2f a, const sf::Vector2f b)
    {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }

    // Returns true if there is a interesection and false otherwise
    // The intersection must be at least n % of edge away from any of edge b points
    inline static bool Intersect(const Edge& a, const Edge& b, double factor)
    {
        const double min = 0.0;
        const double max = 1.0;
        if (factor < min || factor > max)
            return Intersect(a, b[0], b[1]);

        sf::Vector2f intersectionPoint;
        if (GetIntersectionPoint(a, b, intersectionPoint))
        {
            auto distance = Distance(b);
            auto length = Distance(b[0], intersectionPoint);
            auto ratio = length / distance;
            if (ratio > factor && ratio < max - factor)
                return true;
            return false;
        }

        return false;
    }

    // Calculate angle between the line defined by two points and the horizontal axis
    inline static double DifferenceVectorAngle(sf::Vector2f a, sf::Vector2f b)
    {
        const auto deltaY = b.y - a.y; // y axis is upside down
        const auto deltaX = a.x - b.x;
        return atan2(deltaY, deltaX) * 180.0 / M_PI;
    }

    // Casts atan2 result to full degree agle
    inline static double CastAtan2ToFullAngle(double angle)
    {
        return angle < 0 ? (angle + 360) : angle;
    }

    // Calculates length of an edge
    inline static double Distance(const Edge& edge)
    {
        return Distance(edge[0], edge[1]);
    }

    // Returns true if point is inside circle
    inline static bool IsPointInsideCircle(const sf::Vector2f& center, double radius, const sf::Vector2f& point)
    {
        const auto x = std::pow(double(point.x) - center.x, 2.0);
        const auto y = std::pow(double(point.y) - center.y, 2.0);
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

    // Returns true if point is inside polygon, this algorithm uses ray casting
    inline static bool IsPointInsidePolygon(const std::vector<sf::Vector2f>& points, const sf::Vector2f point)
    {
        size_t c = 0;
        const size_t numberOfPoints = points.size();
        for (size_t i = 0, j = numberOfPoints - 1; i < numberOfPoints; j = i++)
        {
            if (((points[i].y > point.y) != (points[j].y > point.y)) &&
                (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
                c = !c;
        }

        // If number of crossed edges is odd the point is inside polygon
        return c == 1;
    }

    // Calculate shape area with given points
    inline static float CalculateArea(const std::vector<sf::Vector2f>& points)
    {
        float area = 0.0f;
        const size_t count = points.size();
        for (size_t i = 0; i < count; ++i)
        {
            size_t j = (i + 1) % count;
            area += points[i].x * points[j].y;
            area -= points[i].y * points[j].x;
        }

        return static_cast<float>(std::fabs(area));
    }

    // Converts SFML position to Box2D position
    inline static b2Vec2 ToBox2DPosition(sf::Vector2f position)
    {
        return b2Vec2(position.x / SFML_BOX2D_SCALE, position.y / SFML_BOX2D_SCALE);
    }

    // Converts Box2D position to SFML position
    inline static sf::Vector2f ToSFMLPosition(b2Vec2 position)
    {
        return sf::Vector2f(position.x * SFML_BOX2D_SCALE, position.y * SFML_BOX2D_SCALE);
    }

    // Converts radians to degrees
    inline static float ToRadians(float degrees)
    {
        return degrees * 0.0174532925199432957f;
    }

    // Converts radians to degrees
    inline static double ToRadians(double degrees)
    {
        return degrees * 0.0174532925199432957;
    }

    // Converts degrees to radians
    inline static float ToDegrees(float radians)
    {
        return radians * 57.295779513082320876f;
    }

    // Converts degrees to radians
    inline static double ToDegrees(double radians)
    {
        return radians * 57.295779513082320876;
    }

    // Returns true if given polygon described in points is convex
    inline static bool IsPolygonConvex(const std::vector<sf::Vector2f>& points)
    {
        const size_t numberOfPoints = points.size();
        int direction = 0;
        int previousDirection = 0;

        for (size_t i = 0; i < numberOfPoints; i++)
        {
            direction = CrossProduct(points[i], points[(i + 1) % numberOfPoints], points[(i + 2) % numberOfPoints]);
            if (direction != 0)
            {
                if (direction * previousDirection < 0)
                    return false;
                previousDirection = direction;
            }
        }

        return true;
    }

    // Checks if given container of edges creates a chain
    inline static bool IsEdgesChain(const EdgeVector& edges)
    {
        if (edges.empty())
            return false;

        const Edge edge = edges.front();
        for (size_t i = 1; i < edges.size(); ++i)
        {
            if (edges[i - 1][1] != edges[i][0])
                return false;

            // If the end of the previous edge is the beggining of the current edge
            // It means that there is no gap
            if (edge[0] == edges[i][1])
            {
                if (i + 1 >= edges.size())
                    return true; // Cycle completed

                return false;
            }
        }

        return false;
    }
};
