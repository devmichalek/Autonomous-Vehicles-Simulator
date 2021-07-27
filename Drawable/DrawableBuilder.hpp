#pragma once
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager;
class DrawableCar;
class DrawableFinishLine;

class DrawableBuilder
{
	bool m_edgeSpecified;
	EdgeVector m_edges;

	bool m_carSpecified;
	sf::Vector2f m_carCenter;
	double m_carAngle;

	bool m_finishLineSpecified;
	Edge m_finishLineSegment;

	EdgeVector m_innerCheckpoints;
	EdgeVector m_outerCheckpoints;

	static inline const std::string m_carAngleString = "Car angle: ";
	static inline const std::string m_carCenterString = "Car center: ";
	static inline const std::string m_finishLineString = "Finish line position: ";
	static inline const std::string m_edgeString = "Edge: ";

	bool getPointFromString(std::string line, sf::Vector2f& result);

	bool getEdgeFromString(std::string line, Edge& result);

	std::vector<size_t> getEdgeSequences();

	bool isOverlappingEdgeSequence(size_t iMin, size_t iMax, size_t jMin, size_t jMax);

	void generateCheckpoints(size_t iMin, size_t iMax, size_t jMin, size_t jMax);

public:
	DrawableBuilder()
	{
		clear();
	}

	~DrawableBuilder()
	{
	}

	void addCar(double angle, sf::Vector2f center);

	void addEdge(Edge edge);

	void addFinishLine(Edge edge);

	// Load map from file
	bool load(const char* filename = "input.txt");

	// Save map to file
	bool save(const char* filename = "output.txt");

	// Clear internal fields
	void clear();

	DrawableManager* getDrawableManager();

	DrawableCar* getDrawableCar();
};