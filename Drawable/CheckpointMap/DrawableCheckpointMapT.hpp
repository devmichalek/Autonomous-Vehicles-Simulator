#pragma once
#include <SFML/Graphics/ConvexShape.hpp>
#include "DrawableMath.hpp"
#include "DrawableCheckpointMap.hpp"

// Triangle algorithm implementation
class DrawableCheckpointMapT :
	public DrawableCheckpointMap
{
public:

	DrawableCheckpointMapT(const EdgeVector& edges, const size_t pivot, const Edge& finishLine);

	void draw();

private:

	Fitness calculateFitness(DetailedCar& car, const Edge& finishLine);

	Fitness getMaxFitness();

	sf::ConvexShape m_shape;

	std::vector<Triangle> m_checkpoints;
};