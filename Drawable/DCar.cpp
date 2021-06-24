#include "DCar.hpp"
#include "CWindow.hpp"

void DCar::accelerate(float value)
{
	m_speed += value;
	if (m_speed > 1.0)
		m_speed = 1.0;
}

void DCar::brake(float value)
{
	m_speed -= value;
	if (m_speed < 0)
		m_speed = 0;
}

void DCar::update()
{
	float cosResult = static_cast<float>(cos(m_angle * M_PI / 180));
	float sinResult = static_cast<float>(sin(m_angle * M_PI / 180));

	m_center.x += m_speed / 10 * cosResult;
	m_center.y += m_speed / 10 * sinResult;

	auto circleRadius = m_circleShape.getRadius();
	m_circleShape.setPosition(m_center - sf::Vector2f(circleRadius, circleRadius));

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
}

void DCar::draw()
{
	CWindow::getRenderWindow().draw(m_convexShape);
	CWindow::getRenderWindow().draw(m_circleShape);
}