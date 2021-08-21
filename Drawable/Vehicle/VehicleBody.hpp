#pragma once
#include "DrawableMath.hpp"

class DrawableVehicle;
class DrawableVehicleBuilder;
class StateVehicleEditor;

class VehicleBody final
{
	friend DrawableVehicle;
	friend DrawableVehicleBuilder;
	friend StateVehicleEditor;

	sf::VertexArray m_vertices;
	std::vector<sf::Vector2f> m_points;

	static sf::Vector2f m_baseCenter;
	static double m_baseSinus;
	static double m_baseCosinus;
	static bool m_initialized;

	const sf::Vector2f* m_center;
	const double* m_sinus;
	const double* m_cosinus;

	VehicleBody();

	~VehicleBody();

	// Initializes static fields
	static void Initialize();

	// Clears dynamicly allocated memory
	void Clear();

	// Set up base components
	void SetBase(const sf::Vector2f* center,
				 const double* sinus,
				 const double* cosinus);

	// Updates each vertex
	void Update();

	// Draws vehicle body
	void Draw();

	// Set this vehicle body with "leader" color
	void SetLeaderColor();

	// Set this vehicle body with "following" color
	void SetFollowerColor();

	// Returns true if given point is inside vehicle body are
	bool Inside(const sf::Vector2f& point);

	// Adds new descriptive point relative to the center
	void AddPoint(sf::Vector2f point);

	// Removes last descriptive point relative to the center
	void RemovePoint(sf::Vector2f point);

	// Returns number of vertices
	size_t GetNumberOfPoints();

	// Return vehicle body described in vertices
	inline const sf::VertexArray& GetVertices()
	{
		return m_vertices;
	}
};
