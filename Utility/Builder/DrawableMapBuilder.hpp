#pragma once
#include "DrawableVehicle.hpp"
#include "AbstractBuilder.hpp"

class DrawableMap;
class DrawableVehicle;

class DrawableMapBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_VEHICLE_IS_NOT_POSITIONED = LAST_ENUM_OPERATION_INDEX,
		ERROR_VEHICLE_OUTSIDE_ALLOWED_MAP_AREA,
		ERROR_VEHICLE_OUTSIDE_ALLOWED_ROAD_AREA,
		ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE,
		ERROR_VEHICLE_ANGLE_IS_TOO_LARGE,
		ERROR_EDGES_ARE_NOT_SPECIFIED,
		ERROR_INCORRECT_EDGE_SEQUENCE_COUNT,
		ERROR_EDGE_SEQUENCE_INTERSECTION,
		ERROR_TOO_LITTLE_INNER_EDGES,
		ERROR_TOO_MANY_INNER_EDGES,
		ERROR_TOO_LITTLE_OUTER_EDGES,
		ERROR_TOO_MANY_OUTER_EDGES,
		ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS
	};

	size_t m_edgesPivot;
	EdgeVector m_edges;
	bool m_vehiclePositioned;
	sf::Vector2f m_vehicleCenter;
	double m_vehicleAngle;

	// Validates if vehicle position is inside allowed area
	bool ValidateAllowedAreaVehiclePosition();

	// Validates if vehicle position is inside road are
	bool ValidateRoadAreaVehiclePosition();

	// Validates if vehicle's angle is correct
	bool ValidateVehicleAngle();

	// Validates total number of edges
	bool ValidateTotalNumberOfEdges(size_t count);

	// Validates edges pivot
	bool ValidateEdgesPivot(size_t pivot, size_t count);

	// Validates number of edge sequences
	bool ValidateNumberOfEdgeSequences();

	// Checks if there is edge sequence intersection
	bool ValidateEdgeSequenceIntersection();

	// Validate triangle checkpoints by creating dummies
	bool ValidateTriangleCheckpoints();

	// Validate internal fields
	bool ValidateInternal();

	// Clears internal fields
	void ClearInternal();

	// Loads map from file
	bool LoadInternal(std::ifstream& input);

	// Saves map to file
	bool SaveInternal(std::ofstream& output);

	// Creates dummy map
	void CreateDummyInternal();

public:

	DrawableMapBuilder();

	~DrawableMapBuilder();

	// Sets intermediate representation of vehicle
	void AddVehicle(double angle, sf::Vector2f center);

	// Adds edge to the intermediate representation of edges container
	void AddEdge(Edge edge);

	// Returns intermediate representation of edges
	EdgeVector GetEdges() const;

	// Updates vehicle position and its angle
	void UpdateVehicle(DrawableVehicle* drawableVehicle);

	// Returns drawable map
	DrawableMap* Get();

	// Copies drawable map
	static DrawableMap* Copy(const DrawableMap* drawableMap);

	// Returns minimum required number of inner edges
	static size_t GetMinNumberOfInnerEdges();

	// Returns maximum required number of inner edges
	static size_t GetMaxNumberOfInnerEdges();

	// Returns minimum required number of outer edges
	static size_t GetMinNumberOfOuterEdges();

	// Returns maximum required number of outer edges
	static size_t GetMaxNumberOfOuterEdges();

	// Returns maximum allowed map area (position + size)
	std::pair<sf::Vector2f, sf::Vector2f> GetMaxAllowedMapArea() const;

	// Returns maximum allowed view area (position + size)
	std::pair<sf::Vector2f, sf::Vector2f> GetMaxAllowedViewArea() const;

	// Returns minimum vehicle's angle
	static double GetMinVehicleAngle();

	// Returns maximum vehicle's angle
	static double GetMaxVehicleAngle();

	// Checks if dummy can be created
	static bool Initialize();
};