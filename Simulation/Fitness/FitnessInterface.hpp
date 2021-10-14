#pragma once
#include "Genetic.hpp"

class FitnessInterface
{
public:

	// Sets fitness
	inline void SetFitness(Fitness fitness)
	{
		m_fitness = fitness;
	}

	// Returns  fitness
	inline Fitness GetFitness() const
	{
		return m_fitness;
	}

protected:

	FitnessInterface() :
		m_fitness(0.0)
	{
	}

	FitnessInterface(Fitness fitness) :
		m_fitness(fitness)
	{
	}

	virtual ~FitnessInterface()
	{
	}

private:

	Fitness m_fitness;
};