#pragma once
#include "DrawableVehicle.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableMap
{
public:

	DrawableMap(EdgeVector edges, size_t pivot);

	~DrawableMap();

	void Intersect(DrawableVehicleFactory& drawableVehicleFactory);

	void Draw();

private:

	Line m_edgeLine;
	EdgeVector m_edges;
};