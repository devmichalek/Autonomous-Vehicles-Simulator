#pragma once
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"

class VehicleBody final
{
	VehicleBody();

	~VehicleBody();

	// Initializes static fields
	static void Initialize();

	// Clears dynamically allocated memory
	void Clear();

	// Set up base components
	void SetBase(const sf::Vector2f* center,
		const double* sinus,
		const double* cosinus);

	// Updates each vertex
	void Update();

	// Draws vehicle body
	void Draw()
	{
		CoreWindow::GetRenderWindow().draw(m_vertices);
	}

	// Set this vehicle body with "leader" color
	void SetLeaderColor();

	// Set this vehicle body with "following" color
	void SetFollowerColor();

	// Returns true if given point is inside vehicle body area
	bool Inside(const sf::Vector2f& point) const;

	// Adds new descriptive point relative to the center
	void AddPoint(sf::Vector2f point);

	// Removes part of vehicle body if the given point relative to the center is inside triangle
	void RemovePoint(sf::Vector2f point);

	// Returns number of vertices
	inline size_t GetNumberOfPoints() const
	{
		return m_points.size();
	}

	// Return vehicle body described in vertices
	inline const sf::VertexArray& GetVertices() const
	{
		return m_vertices;
	}

	sf::VertexArray m_vertices;
	std::vector<sf::Vector2f> m_points;

	static sf::Vector2f m_baseCenter;
	static double m_baseSinus;
	static double m_baseCosinus;
	static bool m_initialized;

	const sf::Vector2f* m_center;
	const double* m_sinus;
	const double* m_cosinus;

	// Friend classes
	friend class DrawableVehicle;
	friend class DrawableVehicleBuilder;
	friend class StateVehicleEditor;
};
