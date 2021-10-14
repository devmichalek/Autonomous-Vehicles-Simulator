#pragma once
#include "SimulatedAbstract.hpp"
#include "FitnessInterface.hpp"
#include "DrawableCheckpoint.hpp"

class SimulatedCheckpoint final :
	public SimulatedAbstract,
	public FitnessInterface,
	public DrawableCheckpoint
{
public:

	SimulatedCheckpoint(size_t identity, Triangle position) :
		SimulatedAbstract(SimulatedAbstract::CategoryCheckpoint),
		FitnessInterface(Fitness(identity + 1)),
		DrawableCheckpoint(sf::Color(255 * (identity % 3), 255 * ((identity + 1) % 3), 255 * ((identity + 2) % 3), 48), position)
	{
	}
};