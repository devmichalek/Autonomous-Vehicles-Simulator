#pragma once
#include <math.h>
#include <array>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "CoreWindow.hpp"
#include "DrawableMath.hpp"

using CarPoints = std::array<sf::Vector2f, 4>;

class DrawableCar
{
	double m_angle;
	const double m_constRotation;
	float m_speed;
	sf::Vector2f m_size;
	sf::Vector2f m_center;
	sf::ConvexShape m_convexShape;
	sf::CircleShape m_circleShape;

public:
	DrawableCar(sf::Vector2f center = sf::Vector2f(0.0f, 0.0f)) :
		m_constRotation(100.0), m_center(center)
	{
		m_angle = 0;
		m_speed = 0;
		auto windowSize = CoreWindow::getSize();
		m_size = sf::Vector2f(windowSize.x / 30.0f, windowSize.y / 10.0f);
		m_convexShape.setPointCount(4);
		m_circleShape.setRadius(m_size.x / 20.0f);
		m_circleShape.setFillColor(sf::Color::Red);
		update();
	}

	virtual ~DrawableCar()
	{
	}

	// Sets color
	inline void setColor(const sf::Color color)
	{
		m_convexShape.setFillColor(color);
	}

	// Sets center position
	inline void setCenter(const sf::Vector2f center)
	{
		m_center = center;
	}

	inline sf::Vector2f getCenter()
	{
		return m_center;
	}

	// Rotate car by specified value (-1; 1)
	void rotate(double rotationRatio);

	// Accelerate by specified value (0; 1)
	void accelerate(float value);

	// Brake by specified value (0; 1)
	void brake(float value);
	
	// Update car rotation and position
	void update();

	// Draws car
	void draw();

	inline float getArea() const
	{
		return m_size.x * m_size.y;
	}

	inline double getAngle() const
	{
		return m_angle;
	}

	// Returns car described in four points
	inline CarPoints getPoints()
	{
		return {
			m_convexShape.getPoint(0),
			m_convexShape.getPoint(1),
			m_convexShape.getPoint(2),
			m_convexShape.getPoint(3)
		};
	}

	inline bool intersect(sf::Vector2f P)
	{
		CarPoints points = getPoints();
		sf::Vector2f& A = points[0];
		sf::Vector2f& B = points[1];
		sf::Vector2f& C = points[2];
		sf::Vector2f& D = points[3];

		float ABP = calculateTriangleArea(A, B, P);
		float BCP = calculateTriangleArea(B, C, P);
		float CDP = calculateTriangleArea(C, D, P);
		float DAP = calculateTriangleArea(D, A, P);

		float area = ABP + BCP + CDP + DAP;
		float correctArea = getArea() + 1;
		return area <= correctArea;
	}
};
