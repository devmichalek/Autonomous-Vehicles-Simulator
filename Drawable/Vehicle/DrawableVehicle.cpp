#include "DrawableVehicle.hpp"
#include "CoreWindow.hpp"

DrawableVehicle::DrawableVehicle(VehicleBody vehicleBody, VehicleSensors vehicleSensors) :
	m_angle(0.0),
	m_speed(0.0),
	m_center(sf::Vector2f(0.0f, 0.0f)),
	m_vehicleBody(vehicleBody),
	m_vehicleSensors(vehicleSensors)
{
	Update();
}

DrawableVehicle::~DrawableVehicle()
{
}

void DrawableVehicle::Initialize()
{
	VehicleSensors::Initialize();
}

void DrawableVehicle::Rotate(Neuron value)
{
	value *= 2.0;
	value -= 1.0;
	m_angle += value * m_rotationConst * CoreWindow::GetElapsedTime();
}

void DrawableVehicle::Accelerate(Neuron value)
{
	m_speed += (value / m_speedFactor);
}

void DrawableVehicle::Brake(Neuron value)
{
	m_speed -= (value / m_speedFactor);
}

void DrawableVehicle::Update()
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

	m_vehicleBody.Update(m_center, m_angle);
	m_vehicleSensors.Update(m_center, m_angle);
}

void DrawableVehicle::DrawBody()
{
	m_vehicleBody.Draw();
	m_vehicleSensors.DrawSensors();
}

void DrawableVehicle::DrawBeams()
{
	m_vehicleSensors.DrawBeams();
}