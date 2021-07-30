#pragma once
#include "DrawableCheckpointMap.hpp"

// Right angle algorithm implementation
class DrawableCheckpointMapRA :
	public DrawableCheckpointMap
{
public:

	DrawableCheckpointMapRA(const EdgeVector& edges, const size_t pivot, const Edge& finishLine);

	Fitness calculateFitness(DetailedCar& car, const Edge& finishLine);

	Fitness getMaxFitness();

	void draw();

private:

	EdgeVector m_innerCheckpoints;

	EdgeVector m_outerCheckpoints;
};