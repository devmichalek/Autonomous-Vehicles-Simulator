#include "DrawableVehicle.hpp"
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"
#include "VehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include <Box2D\Box2D.h>


DrawableVehicle::DrawableVehicle(size_t numberOfBodyPoints, size_t numberOfSensors)
{
	m_bodyShape.setPointCount(numberOfBodyPoints);
	m_beams.resize(numberOfSensors);
	for (auto& beam : m_beams)
	{
		beam[0].color = sf::Color(255, 255, 255, 144);
		beam[1].color = sf::Color(255, 255, 255, 32);
	}
	
	m_sensorShape.setRadius(VehicleBuilder::GetDefaultSensorSize().x);
	m_sensorShape.setFillColor(sf::Color(0xAA, 0x4A, 0x44, 0xFF));
}

void DrawableVehicle::Draw(b2Fixture* fixture)
{
	// Draw body
	CoreWindow::GetRenderWindow().draw(m_bodyShape);

	// Draw sensors and its beams
	for (auto & beam : m_beams)
	{
		// Draw beam shape
		CoreWindow::GetRenderWindow().draw(beam.data(), beam.size(), sf::Lines);

		// Draw sensor shape
		m_sensorShape.setPosition(beam[0].position - VehicleBuilder::GetDefaultSensorSize());
		CoreWindow::GetRenderWindow().draw(m_sensorShape);
	}
}
