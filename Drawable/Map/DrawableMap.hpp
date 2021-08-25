#pragma once
#include "DrawableVehicle.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableMap
{
public:

	DrawableMap(EdgeVector edges, size_t pivot);

	~DrawableMap();

	// Checks if there is an intersection of vehicles against map (edges)
	void Intersect(DrawableVehicleFactory& drawableVehicleFactory);

	// Draws map (edges)
	void Draw();

private:

	Line m_edgeLine;
	EdgeVector m_edges;
};