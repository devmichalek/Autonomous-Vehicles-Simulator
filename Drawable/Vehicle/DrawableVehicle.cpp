#include "DrawableVehicle.hpp"
#include "CoreWindow.hpp"

DrawableVehicle::DrawableVehicle(VehicleBody vehicleBody, VehicleSensors vehicleSensors) :
	m_active(true),
	m_angle(0.0),
	m_sinus(0.0),
	m_cosinus(0.0),
	m_speed(0.0),
	m_center(sf::Vector2f(0.0f, 0.0f)),
	m_vehicleBody(vehicleBody),
	m_vehicleSensors(vehicleSensors)
{
	m_vehicleBody.SetBase(&m_center, &m_sinus, &m_cosinus);
	m_vehicleSensors.SetBase(&m_center, &m_angle, &m_sinus, &m_cosinus);
	Update();
}

DrawableVehicle::~DrawableVehicle()
{
}

void DrawableVehicle::Initialize()
{
	VehicleBody::Initialize();
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

	m_cosinus = cos(m_angle * M_PI / 180);
	m_sinus = sin(m_angle * M_PI / 180);
	double elapsedTime = CoreWindow::GetElapsedTime();
	m_center.x += static_cast<float>(m_speed * elapsedTime * m_speedFactor * m_cosinus);
	m_center.y += static_cast<float>(m_speed * elapsedTime * m_speedFactor * m_sinus);
	m_speed -= elapsedTime;

	m_vehicleBody.Update();
	m_vehicleSensors.Update();
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