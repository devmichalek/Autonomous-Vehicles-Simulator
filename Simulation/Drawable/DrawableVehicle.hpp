#pragma once
#include "DrawableInterface.hpp"
#include "DrawableMath.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class DrawableVehicle :
	public DrawableInterface
{
protected:

	sf::ConvexShape m_bodyShape;
	std::vector<EdgeShape> m_beams;
	sf::CircleShape m_sensorShape;
	sf::Color m_defaultColor; // Default color representing mass

public:

	DrawableVehicle(size_t numberOfBodyPoints, size_t numberOfSensors);

	~DrawableVehicle()
	{
	}

	// Draws vehicle based on provided fixture
	void Draw(b2Fixture* fixture);

	// Set this vehicle with leader color
	inline void SetLeaderColor()
	{
		m_bodyShape.setFillColor(sf::Color(0x60, 0x83, 0x41, 0xFF));
	}

	// Set this vehicle with default color
	inline void SetDefaultColor()
	{
		m_bodyShape.setFillColor(m_defaultColor);
	}
};
