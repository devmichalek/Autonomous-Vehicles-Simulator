#pragma once
#include "DrawableCheckpointMap.hpp"

// Right angle algorithm implementation
class DrawableCheckpointMapRA :
	public DrawableCheckpointMap
{
public:

	DrawableCheckpointMapRA(const EdgeVector& edges, const size_t pivot, const Edge& finishLine);

private:

	Fitness calculateFitness(DetailedCar& car, const Edge& finishLine);

	Fitness getMaxFitness();

	size_t m_innerMax;
};