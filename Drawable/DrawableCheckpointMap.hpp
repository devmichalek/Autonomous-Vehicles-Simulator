#pragma once
#include <SFML/Graphics/ConvexShape.hpp>
#include "DrawableMath.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"

class StoppableTimer;

class DrawableCheckpointMap final
{
	Fitness getMaxFitness();

	Fitness calculateFitness(DetailedVehicle&);

protected:

	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers;
	double m_minFitnessImprovement;
	sf::ConvexShape m_shape;
	std::vector<Triangle> m_checkpoints;

public:

	DrawableCheckpointMap(const EdgeVector& edges, const size_t pivot);

	~DrawableCheckpointMap();

	void draw();

	void iterate(DetailedVehicleFactory& vehicleFactory);

	size_t markLeader(DetailedVehicleFactory& vehicleFactory);

	void punish(DetailedVehicleFactory& vehicleFactory);
	
	void reset();

	void restart(size_t size, double minFitnessImprovement);

	void incrementTimers();

	const FitnessVector& getFitnessVector() const;

	const Fitness& getHighestFitness() const;

	const Fitness& getHighestFitnessOverall() const;
};