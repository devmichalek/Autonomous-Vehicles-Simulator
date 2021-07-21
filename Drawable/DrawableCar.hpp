#pragma once
#include <math.h>
#include <array>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "CoreWindow.hpp"
#include "DrawableMath.hpp"

class DrawableCar
{
	double m_angle;
	inline static const double m_rotationConst = 100.0;
	float m_speed;
	sf::Vector2f m_size;
	sf::Vector2f m_center;
	sf::ConvexShape m_convexShape;
	sf::Vector2f m_circleShapeSize;
	sf::CircleShape m_circleShape;
	
	enum
	{
		LEFT_SENSOR,
		LEFT_FRONT_SENSOR,
		FRONT_SENSOR,
		RIGHT_FRONT_SENSOR,
		RIGHT_SENSOR,
	};
	std::array<Segment, 5> m_beams;
	std::array<double, 5> m_beamAngles;
	Line m_line;
	float m_beamReach;

public:
	DrawableCar(double angle = 0.0, sf::Vector2f center = sf::Vector2f(0.0f, 0.0f)) :
		m_angle(angle), m_center(center), m_speed(0.0), m_beamAngles({ 180.0, 225.0, 270.0, 315.0, 0.0})
	{
		auto windowSize = CoreWindow::getSize();
		const float widthFactor = 30.0f;
		const float heightFactor = 10.0f;
		m_size = sf::Vector2f(windowSize.x / widthFactor, windowSize.y / heightFactor);
		m_convexShape.setPointCount(4);
		const float factor = 20.0f;
		m_circleShapeSize = sf::Vector2f(m_size.x / factor, m_size.x / factor);
		m_circleShape.setRadius(m_circleShapeSize.x);
		m_circleShape.setFillColor(sf::Color::Red);
		m_line[0].color = sf::Color(255, 255, 255, 144);
		m_line[1].color = sf::Color(255, 255, 255, 0);
		m_beamReach = float(windowSize.y) * 0.75f;
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
	inline RectanglePoints getPoints()
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
		RectanglePoints points = getPoints();
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
