#include "DrawableCar.hpp"
#include "CoreWindow.hpp"

void DrawableCar::rotate(double rotationRatio)
{
	rotationRatio *= 2.0;
	rotationRatio -= 1.0;
	m_angle += rotationRatio * m_rotationConst * CoreWindow::getElapsedTime();
}

void DrawableCar::accelerate(double value)
{
	m_speed += (value / m_speedConst);
}

void DrawableCar::brake(double value)
{
	m_speed -= (value / m_speedConst);
}

void DrawableCar::update()
{
	double cosResult = cos(m_angle * M_PI / 180);
	double sinResult = sin(m_angle * M_PI / 180);

	if (m_speed > m_maxSpeedConst)
		m_speed = m_maxSpeedConst;

	if (m_speed < m_minSpeedConst)
		m_speed = m_minSpeedConst;

	double elapsedTime = CoreWindow::getElapsedTime();
	m_center.x += static_cast<float>(m_speed * elapsedTime * m_speedConst * cosResult);
	m_center.y += static_cast<float>(m_speed * elapsedTime * m_speedConst * sinResult);
	m_speed -= elapsedTime;

	m_points[0] = sf::Vector2f(-m_size.x / 2, -m_size.y / 2);
	m_points[1] = sf::Vector2f(m_size.x / 2, -m_size.y / 2);
	m_points[2] = sf::Vector2f(m_size.x / 2, m_size.y / 2);
	m_points[3] = sf::Vector2f(-m_size.x / 2, m_size.y / 2);
	
	for (auto i = 0; i < m_convexShape.getPointCount(); ++i)
	{
		auto& point = m_points[i];
		float rotatedX = static_cast<float>(double(point.x) * cosResult - double(point.y) * sinResult);
		float rotatedY = static_cast<float>(double(point.x) * sinResult + double(point.y) * cosResult);
		point.x = rotatedX + m_center.x;
		point.y = rotatedY + m_center.y;
	}

	// Set sensor beams start point
	m_beams[LEFT_SENSOR][0] = midpoint(m_points[0], m_points[1]);
	m_beams[LEFT_FRONT_SENSOR][0] = m_points[1];
	m_beams[FRONT_SENSOR][0] = midpoint(m_points[1], m_points[2]);
	m_beams[RIGHT_FRONT_SENSOR][0] = m_points[2];
	m_beams[RIGHT_SENSOR][0] = midpoint(m_points[2], m_points[3]);

	// Set sensor beams end point
	for (size_t i = 0; i < 5; ++i)
	{
		float cosBeam = static_cast<float>(cos((m_angle + m_beamAngles[i]) * M_PI / 180));
		float sinBeam = static_cast<float>(sin((m_angle + m_beamAngles[i]) * M_PI / 180));
		m_beams[i][1].x = (m_beams[i][0].x + m_beamReach * cosBeam);
		m_beams[i][1].y = (m_beams[i][0].y + m_beamReach * sinBeam);
	}

	// Set convex shape points
	for (size_t i = 0; i < m_convexShape.getPointCount(); ++i)
		m_convexShape.setPoint(i, m_points[i]);
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