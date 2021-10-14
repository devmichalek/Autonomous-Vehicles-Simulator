#pragma once
#include "DrawableInterface.hpp"
#include "DrawableMath.hpp"
#include "VehicleBuilder.hpp"

class DrawableVehicle :
	public DrawableInterface
{
protected:

	sf::ConvexShape m_bodyShape;
	std::vector<EdgeShape> m_beams;
	sf::CircleShape m_sensorShape;
	sf::Color m_defaultColor; // Default color representing mass

	DrawableVehicle(size_t numberOfBodyPoints, size_t numberOfSensors)
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

	~DrawableVehicle()
	{
	}

	// Sets default color that will be used to color body
	inline void SetDefaultColor(const float mass)
	{
		const float ratio = mass / VehicleBuilder::GetMaxVehicleMass();
		const sf::Uint8 ceiling = sf::Uint8(0xFF * 0.9f);
		const sf::Uint8 part = 0xFF - sf::Uint8(ceiling * ratio);
		m_defaultColor = sf::Color(part, part, part, 0xFF);
		m_bodyShape.setFillColor(m_defaultColor);
	}

public:

	// Set this vehicle with follower color
	inline void SetAsFollower()
	{
		m_bodyShape.setFillColor(m_defaultColor);
	}

	// Set this vehicle with leader color
	inline void SetAsLeader()
	{
		m_bodyShape.setFillColor(sf::Color(0x60, 0x83, 0x41, 0xFF));
	}

	// Draws vehicle
	void Draw()
	{
		// Draw body
		CoreWindow::GetRenderWindow().draw(m_bodyShape);

		// Draw sensors and its beams
		for (auto& beam : m_beams)
		{
			CoreWindow::GetRenderWindow().draw(beam.data(), beam.size(), sf::Lines);
			m_sensorShape.setPosition(beam[0].position - VehicleBuilder::GetDefaultSensorSize());
			CoreWindow::GetRenderWindow().draw(m_sensorShape);
		}
	}
};
