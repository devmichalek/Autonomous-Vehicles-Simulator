#pragma once
#include "DrawableVehicle.hpp"
#include "Genetic.hpp"
#include "StoppableTimer.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class DrawableVehicle;
class DrawableMapBuilder;

class DrawableMap final
{

	DrawableMap(const EdgeVector& edges, const size_t pivot);

	DrawableMap(const DrawableMap& drawableMap);

public:

	~DrawableMap();

	// Reset fitness vector and timers
	void Reset();

	// Initiates internal fields, resizes vectors
	void Init(size_t size, double minFitnessImprovement);

	// Checks if there is an intersection of vehicles against map (edges)
	void Intersect(DrawableVehicleFactory& drawableVehicleFactory);

	// Draws map (edges)
	void Draw();

	// Draws checkpoints
	void DrawCheckpoints();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(DrawableVehicleFactory& drawableVehicleFactory);

	// Marks leader of current iteration (drawable vehicle with highest fitness)
	size_t MarkLeader(DrawableVehicleFactory& drawableVehicleFactory);

	// Checks if drawable vehicle has made improvement, if not then drawable vehicle is set as inactive
	std::pair<size_t, double> Punish(DrawableVehicleFactory& drawableVehicleFactory);

	// Returns fitness vector
	const FitnessVector& GetFitnessVector() const;

	// Return highest fitness in the current iteration
	double GetHighestFitness();

	// Returns highest fitness that has been recorded so far
	double GetHighestFitnessOverall();

	// Calculates fitness of drawable vehicle
	Fitness CalculateFitness(DrawableVehicle* drawableVehicle);

	static TriangleVector GenerateTriangleCheckpoints(const EdgeVector& edges, const size_t pivot);

private:

	// Returns maximum fitness
	Fitness GetMaxFitness();

	using EndPoint = std::pair<size_t, size_t>;
	using EndPoints = std::vector<EndPoint>;
	using EndPointsVector = std::vector<EndPoints>;
	using EdgePrecedence = std::pair<Edge, size_t>;
	using EdgePrecedences = std::vector<EdgePrecedence>;
	using EdgePrecedencesVector = std::vector<EdgePrecedences>;
	friend DrawableMapBuilder;

	static EndPoints GetEndPoints(const EdgeVector& edges, const size_t pivot, const size_t index);

	static EdgePrecedencesVector GetEdgePrecedencesVector(const EdgeVector& edges, const size_t pivot, EndPointsVector& endPointsVector);

	static TriangleVector GetTriangleCheckpoints(const EdgeVector& edges, const EdgePrecedencesVector& edgePrecedencesVector);

	// Map data
	const size_t m_edgesPivot;
	Line m_edgeLine;
	EdgeVector m_edges;

	// Fitness data
	FitnessVector m_fitnessVector;
	std::vector<double> m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers;
	double m_minFitnessImprovement;

	// Checkpoints data
	sf::ConvexShape m_triangleCheckpointShape;
	TriangleVector m_triangleCheckpoints;
};