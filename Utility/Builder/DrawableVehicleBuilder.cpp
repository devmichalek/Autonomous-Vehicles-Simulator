#include "DrawableVehicleBuilder.hpp"

bool DrawableVehicleBuilder::Validate()
{
	return true;
}

void DrawableVehicleBuilder::Clear()
{

}

bool DrawableVehicleBuilder::CreateDummy()
{
	Clear();

	auto windowSize = CoreWindow::GetSize();
	const float widthFactor = 30.0f;
	const float heightFactor = 10.0f;
	sf::Vector2f m_size = sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);

	m_vehicleBody.AddPoint(sf::Vector2f(-m_size.x / 2, -m_size.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(m_size.x / 2, -m_size.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(m_size.x / 2, m_size.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(-m_size.x / 2, m_size.y / 2));

	m_vehicleSensors.AddSensor(sf::Vector2f(0, -m_size.y / 2), 270.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(m_size.x / 2, -m_size.y / 2), 315.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(m_size.x / 2, 0), 0);
	m_vehicleSensors.AddSensor(sf::Vector2f(m_size.x / 2, m_size.y / 2), 45.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(0, m_size.y / 2), 90.0);

	// Validate
	return Validate();
}

DrawableVehicle* DrawableVehicleBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return new DrawableVehicle(m_vehicleBody, m_vehicleSensors);
}

