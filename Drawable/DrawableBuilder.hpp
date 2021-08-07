#pragma once
#include "DrawableCar.hpp"

class DrawableEdgeManager;
class DrawableCar;
class DrawableCheckpointMap;

class DrawableBuilder
{
	size_t m_edgesPivot;
	EdgeVector m_edges;
	bool m_carSpecified;
	sf::Vector2f m_carCenter;
	double m_carAngle;
	bool m_validated;

	static inline const std::string m_carAngleString = "Car angle: ";
	static inline const std::string m_carCenterString = "Car center: ";
	static inline const std::string m_edgeString = "Edge: ";

	bool GetPointFromString(std::string line, sf::Vector2f& result);

	bool GetEdgeFromString(std::string line, Edge& result);

	std::vector<size_t> CountEdgeSequences();

	bool IsEdgeSequenceIntersection(std::vector<size_t>);

	bool ValidateEdges();

public:

	DrawableBuilder();

	~DrawableBuilder()
	{
	}

	void AddCar(double angle, sf::Vector2f center);

	void AddEdge(Edge edge);

	// Load map from file
	bool Load(const char* filename = "input.txt");

	// Save map to file
	bool Save(const char* filename = "output.txt");

	// Clear internal fields
	void Clear();

	DrawableEdgeManager* GetDrawableManager();

	DrawableCar* GetDrawableCar();

	DrawableCheckpointMap* GetDrawableCheckpointMap();
};