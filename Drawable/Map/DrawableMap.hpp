#pragma once
#include "DrawableVehicle.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class DrawableVehicle;
class DrawableMapBuilder;

class DrawableMap final
{
	friend DrawableMapBuilder;

	DrawableMap(const EdgeVector& edges, const size_t pivot);

public:

	~DrawableMap();

	// Resets internal fields
	void Reset();

	// Initiates internal fields, resizes vectors
	void Init(size_t size, double minFitnessImprovement);

	// Checks if there is an intersection of vehicles against map (edges)
	void Intersect(DrawableVehicleFactory& drawableVehicleFactory);

	// Draws map (edges)
	void Draw();

	// Draws checkpoint map
	void DrawDebug();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(DrawableVehicleFactory& drawableVehicleFactory);

	// Marks leader of current iteration (drawable vehicle with highest fitness)
	size_t MarkLeader(DrawableVehicleFactory& drawableVehicleFactory);

	// Checks if drawable vehicle has made improvement, if not then drawable vehicle is set as inactive
	std::pair<size_t, double> Punish(DrawableVehicleFactory& drawableVehicleFactory);

	// Update timers
	void UpdateTimers();

	// Returns fitness vector
	const FitnessVector& GetFitnessVector() const;

	// Return highest fitness in the current iteration
	const Fitness& GetHighestFitness() const;

	// Returns highest fitness that has been recorded so far
	const Fitness& GetHighestFitnessOverall() const;

	// Calculates fitness of drawable vehicle
	Fitness CalculateFitness(DrawableVehicle* drawableVehicle);

private:

	// Returns maximum fitness
	Fitness GetMaxFitness();

	// Fitness data
	FitnessVector m_fitnessVector;
	std::vector<double> m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers;
	double m_minFitnessImprovement;

	// Checkpoints data
	sf::ConvexShape m_shape;
	std::vector<Triangle> m_checkpoints;

	// Map data
	Line m_edgeLine;
	EdgeVector m_edges;
	const size_t m_pivot;
};