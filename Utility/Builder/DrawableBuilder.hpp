#pragma once
#include "DrawableCar.hpp"
#include <map>

class DrawableEdgeManager;
class DrawableCar;
class DrawableCheckpointMap;

class DrawableBuilder final
{
	size_t m_edgesPivot;
	EdgeVector m_edges;
	bool m_carSpecified;
	sf::Vector2f m_carCenter;
	double m_carAngle;
	bool m_validated;

	enum
	{
		UNKNOWN,
		SUCCESS_LOAD_COMPLETED,
		SUCCESS_SAVE_COMPLETED,
		SUCCESS_VALIDATION_PASSED,
		ERROR_VEHICLE_IS_NOT_SPECIFIED,
		ERROR_EDGES_ARE_NOT_SPECIFIED,
		ERROR_INCORRECT_EDGE_SEQUENCE_COUNT,
		ERROR_EDGE_SEQUENCE_INTERSECTION,
		ERROR_TOO_LITTLE_INNER_EDGES,
		ERROR_TOO_LITTLE_OUTER_EDGES,
		ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING,
		ERROR_CANNOT_OPEN_FILE_FOR_READING,
		ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING,
		ERROR_CANNOT_EXTRACT_POINT_WHILE_READING,
		ERROR_CANNOT_EXTRACT_EDGE_WHILE_READING,
		ERROR_CANNOT_FIND_CAR_ANGLE_STRING_WHILE_READING,
		ERROR_CANNOT_FIND_CAR_CENTER_STRING_WHILE_READING,
		ERROR_CANNOT_FIND_EDGE_STRING_WHILE_READING,
		ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING,
		ERROR_CANNOT_OPEN_FILE_FOR_WRITING,
	};
	std::map<size_t, std::string> m_operationsMap;
	size_t m_lastOperationStatus;

	// String constants used for file input/output operations
	static inline const std::string m_carAngleString = "Car angle: ";
	static inline const std::string m_carCenterString = "Car center: ";
	static inline const std::string m_edgeString = "Edge: ";

	// Validates internal fields
	bool Validate();

public:

	DrawableBuilder();

	~DrawableBuilder();

	// Clears internal fields
	void Clear();

	// Sets intermediate representation of car
	void AddCar(double angle, sf::Vector2f center);

	// Adds edge to the intermediate representation of edges container
	void AddEdge(Edge edge);

	// Loads map from file
	bool Load(std::string filename);

	// Saves map to file
	bool Save(std::string filename);

	// Returns last operation status
	// Returns true in case of success and false in case of failure
	std::pair<bool, std::string> GetLastOperationStatus();

	// Returns intermediate representation of edges
	EdgeVector GetEdges();

	// Returns intermediate representation of vehicle
	std::pair<sf::Vector2f, double> GetCar();

	DrawableEdgeManager* GetDrawableManager();

	DrawableCar* GetDrawableCar();

	DrawableCheckpointMap* GetDrawableCheckpointMap();
};