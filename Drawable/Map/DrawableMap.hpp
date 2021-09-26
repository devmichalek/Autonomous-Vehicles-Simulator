#pragma once
#include "DrawableVehicle.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class DrawableMap final
{
	DrawableMap(const EdgeVector& edges, const size_t pivot);

	DrawableMap(const DrawableMap& drawableMap);

public:

	~DrawableMap();

	// Resets fitness vector (its elements are set to 0) and timers
	void Reset();

	// Initiates internal fields, resizes vectors
	void Init(size_t size, double minFitnessImprovement);

	// Checks if there is an intersection vehicles vs edges
	// Update timers
	void Update(DrawableVehicleFactory& drawableVehicleFactory);

	// Draws map (edges)
	void Draw();

	// Draws checkpoints
	void DrawCheckpoints();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(DrawableVehicleFactory& drawableVehicleFactory);

	// Marks leader of current iteration (drawable vehicle with highest fitness)
	size_t MarkLeader(DrawableVehicleFactory& drawableVehicleFactory);

	// Checks if drawable vehicle has made improvement, if not then drawable vehicle is set as inactive
	void Punish(DrawableVehicleFactory& drawableVehicleFactory);

	// Returns fitness vector
	const FitnessVector& GetFitnessVector() const;

	// Return highest fitness in the current iteration in ratio
	Fitness GetHighestFitness() const;

	// Returns highest fitness that has been recorded so far in ratio
	Fitness GetHighestFitnessOverall() const;

	// Returns mean required fitness improvement ratio
	double GetMeanRequiredFitnessImprovement() const;

	// Return number of punished vehicles (non active vehicles)
	size_t GetNumberOfPunishedVehicles() const;

	// Calculates fitness of drawable vehicle
	Fitness CalculateFitness(DrawableVehicle* drawableVehicle);

	// Generates triangle checkpoints for given vector of edges (inner edges + outer edges)
	static TriangleVector GenerateTriangleCheckpoints(const EdgeVector& edges, const size_t pivot);

private:

	// Returns maximum fitness
	Fitness GetMaxFitness() const;

	// Internal types 
	using EndPoint = std::pair<size_t, size_t>;
	using EndPoints = std::vector<EndPoint>;
	using EndPointsVector = std::vector<EndPoints>;

	// Friend classes
	friend class DrawableMapBuilder;

	// Returns available end points from specific point
	// Point is taken from inner edges (first half of edges vector) and it's pointer via index
	// Pivot determines how many inner edges are inside edges vector
	static EndPoints GetEndPoints(const EdgeVector& edges, const size_t pivot, const size_t index);

	// Returns edges vectors representing line checkpoints for each point create from inner edges
	static std::vector<EdgeVector> GetLineCheckpoints(const EdgeVector& edges, const size_t pivot, EndPointsVector& endPointsVector);

	// Transforms edges and edge precedences vector into triangle checkpoints
	static TriangleVector GetTriangleCheckpoints(const std::vector<EdgeVector>& edges);

	// Map data
	const size_t m_edgesPivot;
	Line m_edgeLine;
	EdgeVector m_edges;

	// Fitness data
	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers; // To measure time that has passed since the beggining of iteration for specific instance
	double m_minFitnessImprovement;
	double m_meanRequiredFitnessImprovement;
	size_t m_numberOfPunishedVehicles;

	// Checkpoints data
	sf::ConvexShape m_triangleCheckpointShape;
	TriangleVector m_triangleCheckpoints;
};