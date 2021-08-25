#pragma once
#include "DrawableVehicle.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class StoppableTimer;

class DrawableCheckpointMap final
{
public:

	DrawableCheckpointMap(const EdgeVector& edges, const size_t pivot);

	~DrawableCheckpointMap();

	// Resets internal fields
	void Reset();

	// Initiates internal fields, resizes vectors
	void Init(size_t size, double minFitnessImprovement);

	// Draws checkpoint map
	void Draw();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(DrawableVehicleFactory& drawableVehicleFactory);

	// Marks leader of current iteration (drawable vehicle with highest fitness)
	size_t MarkLeader(DrawableVehicleFactory& drawableVehicleFactory);

	// Checks if drawable vehicle has made improvement, if not then drawable vehicle is set as inactive
	void Punish(DrawableVehicleFactory& drawableVehicleFactory);

	// Update timers
	void UpdateTimers();

	// Returns fitness vector
	const FitnessVector& GetFitnessVector() const;

	// Return highest fitness in the current iteration
	const Fitness& GetHighestFitness() const;

	// Returns highest fitness that has been recorded so far
	const Fitness& GetHighestFitnessOverall() const;

private:

	// Returns maximum fitness
	Fitness GetMaxFitness();

	// Calculates fitness of drawable vehicle
	Fitness CalculateFitness(DrawableVehicle* drawableVehicle);

	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers;
	double m_minFitnessImprovement;
	sf::ConvexShape m_shape;
	std::vector<Triangle> m_checkpoints;
};