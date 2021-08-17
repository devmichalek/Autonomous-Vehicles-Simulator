#pragma once
#include "DrawableMath.hpp"

class DrawableVehicle;
class DrawableVehicleBuilder;

class VehicleBody final
{
	sf::VertexArray m_vertices;
	std::vector<sf::Vector2f> m_points;
	friend DrawableVehicle;
	friend DrawableVehicleBuilder;

	VehicleBody();

	~VehicleBody();

	// Updates each vertex
	void Update(const sf::Vector2f& center, const double& angle);

	// Draws vehicle body
	void Draw();

	// Set this vehicle body with "leader" color
	void SetLeaderColor();

	// Set this vehicle body with "following" color
	void SetFollowerColor();

	// Returns true if given point is inside vehicle body are
	bool Inside(const sf::Vector2f& point, const sf::Vector2f& center);

	// Adds new descriptive point
	void AddPoint(sf::Vector2f point);

	// Removes last descriptive point
	void RemovePoint();

	// Return vehicle body described in vertices
	inline const sf::VertexArray& GetVertices()
	{
		return m_vertices;
	}
};