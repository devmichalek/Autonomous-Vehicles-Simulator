#pragma once
#include <SFML/Graphics/ConvexShape.hpp>
#include "DrawableMath.hpp"
#include "DrawableCheckpointMap.hpp"

// Triangle algorithm implementation
class DrawableCheckpointMapTriangle :
	public DrawableCheckpointMap
{
public:

	DrawableCheckpointMapTriangle(const EdgeVector& edges, const size_t pivot);

	void draw();

private:

	Fitness calculateFitness(DetailedCar& car);

	Fitness getMaxFitness();

	sf::ConvexShape m_shape;

	std::vector<Triangle> m_checkpoints;
};