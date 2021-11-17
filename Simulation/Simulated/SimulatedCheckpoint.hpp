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

	SimulatedCheckpoint(size_t identity, Rectangle position) :
		SimulatedAbstract(SimulatedAbstract::CategoryCheckpoint),
		FitnessInterface(Fitness(identity + 1)),
		DrawableCheckpoint(ColorContext::Create(ColorContext::MaxChannelValue * (identity % 3),
												ColorContext::MaxChannelValue * ((identity + 1) % 3),
												ColorContext::MaxChannelValue * ((identity + 2) % 3),
												96), position)
	{
	}
};