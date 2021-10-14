#pragma once
#include "SimulatedAbstract.hpp"
#include "DrawableEdge.hpp"

class SimulatedEdge final :
	public SimulatedAbstract,
	public DrawableEdge
{
public:

	SimulatedEdge() :
		SimulatedAbstract(SimulatedAbstract::CategoryEdge),
		DrawableEdge()
	{
	}
};