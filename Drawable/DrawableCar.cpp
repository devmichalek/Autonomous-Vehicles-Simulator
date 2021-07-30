#include "DrawableCar.hpp"
#include "CoreWindow.hpp"

void DrawableCar::setLeaderColor()
{
	m_carShape.setFillColor(sf::Color(244, 160, 0, 255));
}

void DrawableCar::setFollowerColor()
{
	m_carShape.setFillColor(sf::Color::White);
}

void DrawableCar::setCenter(const sf::Vector2f center)
{
	m_center = center;
}

sf::Vector2f DrawableCar::getCenter()
{
	return m_center;
}

void DrawableCar::setAngle(double angle)
{
	m_angle = angle;
}

double DrawableCar::getAngle() const
{
	return m_angle;
}

bool DrawableCar::inside(sf::Vector2f point)
{
	float area = GetRectangleArea(m_points, point);
	float correctArea = (m_size.x * m_size.y) + 1;
	return area <= correctArea;
}

void DrawableCar::rotate(Neuron value)
{
	value *= 2.0;
	value -= 1.0;
	m_angle += value * m_rotationConst * CoreWindow::getElapsedTime();
}

void DrawableCar::accelerate(Neuron value)
{
	m_speed += (value / m_speedConst);
}

void DrawableCar::brake(Neuron value)
{
	m_speed -= (value / m_speedConst);
}

void DrawableCar::update()
{
	if (m_speed > m_maxSpeedConst)
		m_speed = m_maxSpeedConst;
	else if (m_speed < m_minSpeedConst)
		m_speed = m_minSpeedConst;

	double cosResult = cos(m_angle * M_PI / 180);
	double sinResult = sin(m_angle * M_PI / 180);
	double elapsedTime = CoreWindow::getElapsedTime();
	m_center.x += static_cast<float>(m_speed * elapsedTime * m_speedConst * cosResult);
	m_center.y += static_cast<float>(m_speed * elapsedTime * m_speedConst * sinResult);
	m_speed -= elapsedTime;

	m_points[0] = sf::Vector2f(-m_size.x / 2, -m_size.y / 2);
	m_points[1] = sf::Vector2f(m_size.x / 2, -m_size.y / 2);
	m_points[2] = sf::Vector2f(m_size.x / 2, m_size.y / 2);
	m_points[3] = sf::Vector2f(-m_size.x / 2, m_size.y / 2);
	
	for (unsigned i = 0; i < CAR_NUMBER_OF_POINTS; ++i)
	{
		auto& point = m_points[i];
		float rotatedX = static_cast<float>(double(point.x) * cosResult - double(point.y) * sinResult);
		float rotatedY = static_cast<float>(double(point.x) * sinResult + double(point.y) * cosResult);
		point.x = rotatedX + m_center.x;
		point.y = rotatedY + m_center.y;
		m_carShape.setPoint(i, point);
	}

	// Set sensor beams start point
	m_beams[0][0] = GetMidpoint(m_points[0], m_points[1]);
	m_beams[1][0] = m_points[1];
	m_beams[2][0] = GetMidpoint(m_points[1], m_points[2]);
	m_beams[3][0] = m_points[2];
	m_beams[4][0] = GetMidpoint(m_points[2], m_points[3]);

	// Set sensor beams end point and sensors value
	for (unsigned i = 0; i < CAR_NUMBER_OF_SENSORS; ++i)
	{
		m_sensors[i] = m_sensorMaxValue;
		auto cosBeam = cos((m_angle + m_beamAngles[i]) * M_PI / 180);
		auto sinBeam = sin((m_angle + m_beamAngles[i]) * M_PI / 180);
		m_beams[i][1].x = static_cast<float>(m_beams[i][0].x + m_beamReach * cosBeam);
		m_beams[i][1].y = static_cast<float>(m_beams[i][0].y + m_beamReach * sinBeam);
	}
}

void DrawableCar::drawBody()
{
	// Draw car
	CoreWindow::getRenderWindow().draw(m_carShape);

	for (const auto& position : m_beams)
	{
		// Draw sensor
		m_sensorShape.setPosition(position[0] - m_sensorSize);
		CoreWindow::getRenderWindow().draw(m_sensorShape);
	}
}

void DrawableCar::drawBeams()
{
	for (const auto& position : m_beams)
	{
		// Draw beam
		m_beamShape[0].position = position[0];
		m_beamShape[1].position = position[1];
		CoreWindow::getRenderWindow().draw(m_beamShape.data(), 2, sf::Lines);
	}
}