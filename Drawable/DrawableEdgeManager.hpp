#pragma once
#include "DrawableCar.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableEdgeManager
{
public:

	DrawableEdgeManager(EdgeVector edges, size_t pivot);

	~DrawableEdgeManager();

	void Intersect(DetailedCarFactory& cars);

	void Draw();

private:

	Line m_edgeLine;
	EdgeVector m_edges;
};