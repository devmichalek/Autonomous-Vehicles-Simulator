#pragma once
#include "SimulatedInterface.hpp"
#include "FitnessInterface.hpp"
#include "DrawableCheckpoint.hpp"

class SimulatedCheckpoint final :
	public SimulatedInterface,
	public FitnessInterface,
	public DrawableCheckpoint
{
public:

	SimulatedCheckpoint(size_t identity, Triangle position) :
		SimulatedInterface(SimulatedInterface::CategoryCheckpoint),
		FitnessInterface(Fitness(identity + 1)),
		DrawableCheckpoint(sf::Color(255 * (identity % 3), 255 * ((identity + 1) % 3), 255 * ((identity + 2) % 3), 48), position)
	{
	}
};