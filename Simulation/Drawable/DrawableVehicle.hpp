#pragma once
#include "DrawableInterface.hpp"
#include "MathContext.hpp"
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
			beam[0].color = ColorContext::BeamBeggining;
			beam[1].color = ColorContext::BeamEnd;
		}
		m_sensorShape.setRadius(VehicleBuilder::GetDefaultSensorSize().x);
		m_sensorShape.setFillColor(ColorContext::VehicleSensorDefault);
	}

	~DrawableVehicle()
	{
	}

	// Sets default color that will be used to color body
	inline void SetDefaultColor(const float mass)
	{
		m_defaultColor = VehicleBuilder::CalculateDefaultColor(mass);
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
		m_bodyShape.setFillColor(ColorContext::LeaderVehicle);
	}

	// Draws vehicle
	void Draw()
	{
		// Draw body
		CoreWindow::Draw(m_bodyShape);

		// Draw sensors and its beams
		for (auto& beam : m_beams)
		{
			CoreWindow::Draw(beam.data(), beam.size(), sf::Lines);
			m_sensorShape.setPosition(beam[0].position - VehicleBuilder::GetDefaultSensorSize());
			CoreWindow::Draw(m_sensorShape);
		}
	}
};
