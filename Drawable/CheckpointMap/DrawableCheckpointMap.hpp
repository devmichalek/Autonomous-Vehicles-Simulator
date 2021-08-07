#pragma once
#include "DrawableMath.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"

class StoppableTimer;

class DrawableCheckpointMap
{
	virtual Fitness getMaxFitness() = 0;

	virtual Fitness calculateFitness(DetailedCar&) = 0;

protected:

	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers;
	double m_minFitnessImprovement;

	DrawableCheckpointMap();

public:

	virtual ~DrawableCheckpointMap();

	virtual void draw() = 0;

	void iterate(DetailedCarFactory& factory);

	size_t markLeader(DetailedCarFactory& factory);

	void punish(DetailedCarFactory& factory);
	
	void reset();

	void restart(size_t size, double minFitnessImprovement);

	void incrementTimers();

	const FitnessVector& getFitnessVector() const;

	const Fitness& getHighestFitness() const;

	const Fitness& getHighestFitnessOverall() const;
};