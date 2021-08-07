#pragma once
#include "DrawableCheckpointMap.hpp"

// Right angle algorithm implementation
class DrawableCheckpointMapBeam :
	public DrawableCheckpointMap
{
public:

	DrawableCheckpointMapBeam(const EdgeVector& edges, const size_t pivot);

	void draw();

private:

	Fitness calculateFitness(DetailedCar& car);

	Fitness getMaxFitness();

	size_t m_innerMax;

	Line m_line;

	EdgeVector m_checkpoints;
};