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
	inline static const double m_rotationConst = 150.0;
	double m_speed;
	inline static const double m_maxSpeedConst = 1.2;
	inline static const double m_minSpeedConst = 0.05;
	inline static const double m_speedConst = 300.0;
	sf::Vector2f m_size;
	sf::Vector2f m_center;
	sf::ConvexShape m_convexShape;
	sf::Vector2f m_circleShapeSize;
	sf::CircleShape m_circleShape;
	CarPoints m_points;
	
	enum
	{
		LEFT_SENSOR,
		LEFT_FRONT_SENSOR,
		FRONT_SENSOR,
		RIGHT_FRONT_SENSOR,
		RIGHT_SENSOR,
	};
	CarBeams m_beams;
	std::array<double, 5> m_beamAngles;
	Line m_line;
	float m_beamReach;

	friend class DrawableManager;

	// Returns car described in four points
	inline CarPoints& getPoints()
	{
		return m_points;
	}

	// Returns sensor beams
	inline CarBeams& getBeams()
	{
		return m_beams;
	}

public:
	DrawableCar(double angle = 0.0, sf::Vector2f center = sf::Vector2f(0.0f, 0.0f)) :
		m_angle(angle), m_center(center), m_speed(0.0), m_beamAngles({ 270.0, 315.0, 0.0, 45.0, 90.0})
	{
		auto windowSize = CoreWindow::getSize();
		const float widthFactor = 30.0f;
		const float heightFactor = 10.0f;
		m_size = sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);
		m_convexShape.setPointCount(4);
		m_circleShapeSize = sf::Vector2f(m_size.x / widthFactor, m_size.x / widthFactor);
		m_circleShape.setRadius(m_circleShapeSize.x);
		m_circleShape.setFillColor(sf::Color::Red);
		m_line[0].color = sf::Color(255, 255, 255, 144);
		m_line[1].color = sf::Color(255, 255, 255, 32);
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

	// Rotate car by specified value (0; 1)
	void rotate(double rotationRatio);

	// Accelerate by specified value (0; 1)
	void accelerate(double value);

	// Brake by specified value (0; 1)
	void brake(double value);
	
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

using DrawableCarFactory = std::vector<std::pair<DrawableCar*, bool>>;
