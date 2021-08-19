#include "DrawableVehicleBuilder.hpp"

bool DrawableVehicleBuilder::Validate()
{
	return true;
}

void DrawableVehicleBuilder::Clear()
{
	m_vehicleBody.Clear();
	m_vehicleSensors.Clear();
	m_validated = false;
}

void DrawableVehicleBuilder::AddVehicleBody(sf::Vector2f point)
{

}

void DrawableVehicleBuilder::AddVehicleSensor(sf::Vector2f point, double angle)
{

}

bool DrawableVehicleBuilder::Load(std::string filename)
{

	return true;
}

bool DrawableVehicleBuilder::Save(std::string filename)
{

	return true;
}

bool DrawableVehicleBuilder::CreateDummy()
{
	Clear();

	sf::Vector2f dummySize = GetMaxVehicleBodySize();
	dummySize /= 2.0f;

	m_vehicleBody.AddPoint(sf::Vector2f(-dummySize.x / 2, -dummySize.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(-dummySize.x / 2, dummySize.y / 2));
	m_vehicleBody.AddPoint(sf::Vector2f(dummySize.x / 2, dummySize.y / 2));

	m_vehicleSensors.AddSensor(sf::Vector2f(0, -dummySize.y / 2), 270.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2), 315.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, 0), 0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, dummySize.y / 2), 45.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(0, dummySize.y / 2), 90.0);

	// Validate
	return Validate();
}

VehicleBody DrawableVehicleBuilder::GetVehicleBody()
{
	return m_vehicleBody;
}

VehicleSensors DrawableVehicleBuilder::GetVehicleSensors()
{
	return m_vehicleSensors;
}

sf::Vector2f DrawableVehicleBuilder::GetMaxVehicleBodySize()
{
	auto windowSize = CoreWindow::GetSize();
	const float widthFactor = 15.0f;
	const float heightFactor = 5;
	return sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);
}

DrawableVehicle* DrawableVehicleBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return new DrawableVehicle(m_vehicleBody, m_vehicleSensors);
}

