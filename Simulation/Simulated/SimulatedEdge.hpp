#pragma once
#include "SimulatedInterface.hpp"
#include "DrawableEdge.hpp"

class SimulatedEdge final :
	public SimulatedInterface,
	public DrawableEdge
{
public:

	SimulatedEdge() :
		SimulatedInterface(SimulatedInterface::CategoryEdge),
		DrawableEdge()
	{
	}
};