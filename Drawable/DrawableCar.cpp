#include "DrawableCar.hpp"
#include "CoreWindow.hpp"

void DrawableCar::rotate(double rotationRatio)
{
	m_angle += rotationRatio * m_rotationConst * CoreWindow::getElapsedTime();
}

void DrawableCar::accelerate(double value)
{
	m_speed += value;
}

void DrawableCar::brake(double value)
{
	m_speed -= (value / m_speedConst);
}

void DrawableCar::update()
{
	float cosResult = static_cast<float>(cos(m_angle * M_PI / 180));
	float sinResult = static_cast<float>(sin(m_angle * M_PI / 180));

	if (m_speed > 1.0)
		m_speed = 1.0;
	if (m_speed < 0)
		m_speed = 0;

	double elapsedTime = CoreWindow::getElapsedTime();
	m_center.x += m_speed * elapsedTime * m_speedConst * cosResult;
	m_center.y += m_speed * elapsedTime * m_speedConst * sinResult;
	m_speed -= elapsedTime;

	m_convexShape.setPoint(0, sf::Vector2f(-m_size.x / 2, -m_size.y / 2));
	m_convexShape.setPoint(1, sf::Vector2f(m_size.x / 2, -m_size.y / 2));
	m_convexShape.setPoint(2, sf::Vector2f(m_size.x / 2, m_size.y / 2));
	m_convexShape.setPoint(3, sf::Vector2f(-m_size.x / 2, m_size.y / 2));
	
	for (auto i = 0; i < m_convexShape.getPointCount(); ++i)
	{
		auto point = m_convexShape.getPoint(i);
		float rotatedX = point.x * cosResult - point.y * sinResult;
		float rotatedY = point.x * sinResult + point.y * cosResult;
		m_convexShape.setPoint(i, sf::Vector2f(rotatedX + m_center.x, rotatedY + m_center.y));
	}

	m_beams[LEFT_SENSOR][0] = midpoint(m_convexShape.getPoint(0), m_convexShape.getPoint(1));
	m_beams[LEFT_FRONT_SENSOR][0] = m_convexShape.getPoint(1);
	m_beams[FRONT_SENSOR][0] = midpoint(m_convexShape.getPoint(1), m_convexShape.getPoint(2));
	m_beams[RIGHT_FRONT_SENSOR][0] = m_convexShape.getPoint(2);
	m_beams[RIGHT_SENSOR][0] = midpoint(m_convexShape.getPoint(2), m_convexShape.getPoint(3));

	for (size_t i = 0; i < 5; ++i)
	{
		float cosBeam = static_cast<float>(cos((m_angle + m_beamAngles[i]) * M_PI / 180));
		float sinBeam = static_cast<float>(sin((m_angle + m_beamAngles[i]) * M_PI / 180));
		m_beams[i][1].x = (m_beams[i][0].x + m_beamReach * cosBeam);
		m_beams[i][1].y = (m_beams[i][0].y + m_beamReach * sinBeam);
	}
}

void DrawableCar::draw()
{
	CoreWindow::getRenderWindow().draw(m_convexShape);
	for (const auto& position : m_beams)
	{
		m_line[0].position = position[0];
		m_line[1].position = position[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		m_circleShape.setPosition(position[0] - m_circleShapeSize);
		CoreWindow::getRenderWindow().draw(m_circleShape);
	}
}