#include "VehicleSensors.hpp"

double VehicleSensors::m_beamLength;
sf::Vector2f VehicleSensors::m_sensorSize;
Line VehicleSensors::m_beamShape;
sf::CircleShape VehicleSensors::m_sensorShape;

VehicleSensors::VehicleSensors()
{
}

VehicleSensors::~VehicleSensors()
{
}

void VehicleSensors::Initialize()
{
	m_beamLength = double(CoreWindow::GetSize().y) * 0.75;
	m_sensorSize.x = CoreWindow::GetSize().x / 400.0f;
	m_sensorSize.y = m_sensorSize.x;
	m_beamShape[0].color = sf::Color(255, 255, 255, 144);
	m_beamShape[1].color = sf::Color(255, 255, 255, 32);
	m_sensorShape.setRadius(m_sensorSize.x);
	m_sensorShape.setFillColor(sf::Color::Red);
}

void VehicleSensors::Update(const sf::Vector2f& center, const double& angle)
{
	double cosValue = cos(angle * M_PI / 180);
	double sinValue = sin(angle * M_PI / 180);

	for (size_t i = 0; i < m_beamVector.size(); ++i)
	{
		// Set sensor max value
		m_sensors[i] = m_sensorMaxValue;

		// Set base
		m_beamVector[i][0] = m_offsetVector[i];

		// Set beam start point
		float x = m_beamVector[i][0].x;
		float y = m_beamVector[i][0].y;
		m_beamVector[i][0].x = static_cast<float>(double(x) * cosValue - double(y) * sinValue);
		m_beamVector[i][0].y = static_cast<float>(double(x) * sinValue + double(y) * cosValue);
		m_beamVector[i][0] += center;

		// Set beam end point
		auto cosBeam = cos((angle + m_angleVector[i]) * M_PI / 180);
		auto sinBeam = sin((angle + m_angleVector[i]) * M_PI / 180);
		m_beamVector[i][1].x = static_cast<float>(m_beamVector[i][0].x + m_beamLength * cosBeam);
		m_beamVector[i][1].y = static_cast<float>(m_beamVector[i][0].y + m_beamLength * sinBeam);
	}
}

void VehicleSensors::DrawBeams()
{
	for (const auto& beam : m_beamVector)
	{
		m_beamShape[0].position = beam[0];
		m_beamShape[1].position = beam[1];
		CoreWindow::GetRenderWindow().draw(m_beamShape.data(), 2, sf::Lines);
	}
}

void VehicleSensors::DrawSensors()
{
	for (const auto& beam : m_beamVector)
	{
		m_sensorShape.setPosition(beam[0] - m_sensorSize);
		CoreWindow::GetRenderWindow().draw(m_sensorShape);
	}
}

void VehicleSensors::AddSensor(sf::Vector2f offset, double angle)
{
	// Push back
	m_beamVector.push_back(Edge());
	m_offsetVector.push_back(offset);
	m_angleVector.push_back(angle);
	m_sensors.push_back(NEURAL_DEFAULT_NEURON_VALUE);

	// Shrink to fit
	m_beamVector.shrink_to_fit();
	m_offsetVector.shrink_to_fit();
	m_angleVector.shrink_to_fit();
	m_sensors.shrink_to_fit();
}

void VehicleSensors::RemoveSensor(sf::Vector2f point)
{
	for (size_t i = 0; i < m_beamVector.size(); ++i)
	{
		if (IsPointInsideCircle(m_beamVector[i][0], m_sensorSize.x, point))
		{
			m_beamVector.erase(m_beamVector.begin() + i);
			m_offsetVector.erase(m_offsetVector.begin() + i);
			m_angleVector.erase(m_angleVector.begin() + i);
			m_sensors.erase(m_sensors.begin() + i);
			--i;
		}
	}
}
