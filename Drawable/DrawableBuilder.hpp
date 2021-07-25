#pragma once
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager;
class DrawableCar;
class DrawableFinishLine;

class DrawableBuilder
{
	bool m_wallSpecified;
	WallVector m_walls;

	bool m_carSpecified;
	sf::Vector2f m_carCenter;
	double m_carAngle;

	bool m_finishLineSpecified;
	Wall m_finishLineSegment;

	static inline const std::string m_carAngleString = "Car angle: ";
	static inline const std::string m_carCenterString = "Car center: ";
	static inline const std::string m_finishLineString = "Finish line position: ";
	static inline const std::string m_wallString = "Wall: ";

	bool getPointFromString(std::string line, sf::Vector2f& result);

	bool getWallFromString(std::string line, Wall& result);

public:
	DrawableBuilder()
	{
		clear();
	}

	~DrawableBuilder()
	{
	}

	void addCar(double angle, sf::Vector2f center);

	void addWall(Wall wall);

	void addFinishLine(Wall wall);

	// Load map from file
	bool load(const char* filename = "input.txt");

	// Save map to file
	bool save(const char* filename = "output.txt");

	// Clear internal fields
	void clear();

	DrawableManager* getDrawableManager();

	DrawableCar* getDrawableCar();
};