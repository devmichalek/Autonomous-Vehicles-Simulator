#pragma once
#include "DrawableVehicle.hpp"
#include "AbstractBuilder.hpp"

class DrawableMap;
class DrawableVehicle;
class DrawableCheckpointMap;

class DrawableMapBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_VEHICLE_IS_NOT_POSITIONED = LAST_ENUM_OPERATION_INDEX,
		ERROR_EDGES_ARE_NOT_SPECIFIED,
		ERROR_INCORRECT_EDGE_SEQUENCE_COUNT,
		ERROR_EDGE_SEQUENCE_INTERSECTION,
		ERROR_TOO_LITTLE_INNER_EDGES,
		ERROR_TOO_LITTLE_OUTER_EDGES,
		ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING,
		ERROR_CANNOT_EXTRACT_POINT_WHILE_READING,
		ERROR_CANNOT_EXTRACT_EDGE_WHILE_READING,
		ERROR_CANNOT_FIND_VEHICLE_ANGLE_STRING_WHILE_READING,
		ERROR_CANNOT_FIND_VEHICLE_CENTER_STRING_WHILE_READING,
		ERROR_CANNOT_FIND_EDGE_STRING_WHILE_READING,
	};

	size_t m_edgesPivot;
	EdgeVector m_edges;
	bool m_vehiclePositioned;
	sf::Vector2f m_vehicleCenter;
	double m_vehicleAngle;

	// String constants used for file input/output operations
	static inline const std::string m_vehicleAngleString = "Vehicle angle: ";
	static inline const std::string m_vehicleCenterString = "Vehicle center: ";
	static inline const std::string m_edgeString = "Edge: ";

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
	EdgeVector GetEdges();

	// Returns intermediate representation of vehicle
	std::pair<sf::Vector2f, double> GetVehicle();

	// Returns drawable manager
	DrawableMap* GetDrawableMap();

	// Returns drawable checkpoint map
	DrawableCheckpointMap* GetDrawableCheckpointMap();

	// Returns minimum required number of inner edges
	size_t GetMinNumberOfInnerEdges() const;

	// Returns maximum required number of inner edges
	size_t GetMaxNumberOfInnerEdges() const;

	// Returns maximum allowed map area
	sf::Vector2f GetMaxAllowedMapArea() const;

	// Returns maximum allowed view area
	sf::Vector2f GetMaxAllowedViewArea() const;
};