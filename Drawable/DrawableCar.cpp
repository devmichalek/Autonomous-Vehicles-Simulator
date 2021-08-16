#include "DrawableCar.hpp"
#include "CoreWindow.hpp"

void DrawableCar::setLeaderColor()
{
	m_body[1].color = sf::Color::Green;
	m_body[2].color = m_body[1].color;
}

void DrawableCar::setFollowerColor()
{
	m_body[1].color = sf::Color::Yellow;
	m_body[2].color = m_body[1].color;
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

void DrawableCar::init(size_t numberOfSensors)
{
	switch (numberOfSensors)
	{
		case CAR_TWELVE_NUMBER_OF_SENSORS:
			m_beamAngles = { 180.0, 225.0, 270.0, 270.0, 315.0, 345.0, 0.0, 15.0, 45.0, 90.0, 90.0, 135.0 };
			m_beamStartPositions.resize(numberOfSensors);
			m_beamStartPositions[0] = std::tuple(0, 3, 0.5);
			m_beamStartPositions[1] = std::tuple(0, 0, 0.5);
			m_beamStartPositions[2] = std::tuple(0, 1, 0.5);
			m_beamStartPositions[3] = std::tuple(1, 1, 0);
			m_beamStartPositions[4] = std::tuple(1, 1, 0);
			m_beamStartPositions[5] = std::tuple(1, 1, 0);
			m_beamStartPositions[6] = std::tuple(1, 2, 0.5);
			m_beamStartPositions[7] = std::tuple(2, 2, 0);
			m_beamStartPositions[8] = std::tuple(2, 2, 0);
			m_beamStartPositions[9] = std::tuple(2, 2, 0);
			m_beamStartPositions[10] = std::tuple(2, 3, 0.5);
			m_beamStartPositions[11] = std::tuple(3, 3, 0.5);
			break;
		case CAR_EIGHT_NUMBER_OF_SENSORS:
			m_beamAngles = { 180.0, 225.0, 270.0, 315.0, 0.0, 45.0, 90.0, 135.0 };
			m_beamStartPositions.resize(numberOfSensors);
			m_beamStartPositions[0] = std::tuple(0, 3, 0.5);
			m_beamStartPositions[1] = std::tuple(0, 0, 0.5);
			m_beamStartPositions[2] = std::tuple(0, 1, 0.5);
			m_beamStartPositions[3] = std::tuple(1, 1, 0);
			m_beamStartPositions[4] = std::tuple(1, 2, 0.5);
			m_beamStartPositions[5] = std::tuple(2, 2, 0);
			m_beamStartPositions[6] = std::tuple(2, 3, 0.5);
			m_beamStartPositions[7] = std::tuple(3, 3, 0.5);
			break;
		default:
		case CAR_FIVE_NUMBER_OF_SENSORS:
			numberOfSensors = CAR_FIVE_NUMBER_OF_SENSORS;
			m_beamAngles = { 270.0, 315.0, 0.0, 45.0, 90.0 };
			m_beamStartPositions.resize(numberOfSensors);
			m_beamStartPositions[0] = std::tuple(0, 1, 0.5);
			m_beamStartPositions[1] = std::tuple(1, 1, 0);
			m_beamStartPositions[2] = std::tuple(1, 2, 0.5);
			m_beamStartPositions[3] = std::tuple(2, 2, 0);
			m_beamStartPositions[4] = std::tuple(2, 3, 0.5);
			break;
	}

	m_beams.resize(numberOfSensors);
	m_sensors.resize(numberOfSensors);
}

void DrawableCar::rotate(Neuron value)
{
	value *= 2.0;
	value -= 1.0;
	m_angle += value * m_rotationConst * CoreWindow::GetElapsedTime();
}

void DrawableCar::accelerate(Neuron value)
{
	m_speed += (value / m_speedFactor);
}

void DrawableCar::brake(Neuron value)
{
	m_speed -= (value / m_speedFactor);
}

void DrawableCar::update()
{
	if (m_speed > m_maxSpeed)
		m_speed = m_maxSpeed;
	else if (m_speed < m_minSpeed)
		m_speed = m_minSpeed;

	double cosResult = cos(m_angle * M_PI / 180);
	double sinResult = sin(m_angle * M_PI / 180);
	double elapsedTime = CoreWindow::GetElapsedTime();
	m_center.x += static_cast<float>(m_speed * elapsedTime * m_speedFactor * cosResult);
	m_center.y += static_cast<float>(m_speed * elapsedTime * m_speedFactor * sinResult);
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
		m_body[i].position = point;
	}

	for (unsigned i = 0; i < m_beams.size(); ++i)
	{
		// Set sensor max value
		m_sensors[i] = m_sensorMaxValue;

		// Set beam start point
		auto& carStartPoint = m_points[std::get<0>(m_beamStartPositions[i])];
		auto& carEndPoint = m_points[std::get<1>(m_beamStartPositions[i])];
		auto length = Distance(carStartPoint, carEndPoint) * std::get<2>(m_beamStartPositions[i]);
		double angle = DifferenceVectorAngle(carStartPoint, carEndPoint);
		m_beams[i][0] = GetEndPoint(m_points[std::get<0>(m_beamStartPositions[i])], angle, float(-length));

		// Set beam end point
		auto cosBeam = cos((m_angle + m_beamAngles[i]) * M_PI / 180);
		auto sinBeam = sin((m_angle + m_beamAngles[i]) * M_PI / 180);
		m_beams[i][1].x = static_cast<float>(m_beams[i][0].x + m_beamReach * cosBeam);
		m_beams[i][1].y = static_cast<float>(m_beams[i][0].y + m_beamReach * sinBeam);
	}
}

void DrawableCar::drawBody()
{
	// Draw car body
	CoreWindow::GetRenderWindow().draw(m_body.data(), CAR_NUMBER_OF_POINTS, sf::Quads);

	// Draw sensors
	for (const auto& position : m_beams)
	{
		m_sensorShape.setPosition(position[0] - m_sensorSize);
		CoreWindow::GetRenderWindow().draw(m_sensorShape);
	}
}

void DrawableCar::drawBeams()
{
	for (const auto& position : m_beams)
	{
		m_beamShape[0].position = position[0];
		m_beamShape[1].position = position[1];
		CoreWindow::GetRenderWindow().draw(m_beamShape.data(), 2, sf::Lines);
	}
}