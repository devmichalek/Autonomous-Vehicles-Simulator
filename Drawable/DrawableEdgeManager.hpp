#pragma once
#include "DrawableVehicle.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableEdgeManager
{
public:

	DrawableEdgeManager(EdgeVector edges, size_t pivot);

	~DrawableEdgeManager();

	void Intersect(DetailedVehicleFactory& vehicleFactory);

	void Draw();

private:

	Line m_edgeLine;
	EdgeVector m_edges;
};