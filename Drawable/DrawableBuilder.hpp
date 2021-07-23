#pragma once
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager;
class DrawableCar;
class DrawableFinishLine;

class DrawableBuilder
{
	bool m_car;
	sf::Vector2f m_carCenter;
	double m_carAngle;

	SegmentVector m_walls;

	bool m_finishLine;
	Segment m_finishLineSegment;

	const char* m_inputFilename = "input.txt";
	const char* m_outputFilename = "output.txt";
	const std::string m_carAngleString = "Car angle: ";
	const std::string m_carCenterString = "Car center: ";
	const std::string m_finishLineString = "Finish line position: ";
	const std::string m_wallString = "Wall: ";

	bool getPointFromString(std::string line, sf::Vector2f& result);
	bool getSegmentFromString(std::string line, Segment& result);

public:
	DrawableBuilder()
	{
		clear();
	}

	~DrawableBuilder()
	{
	}

	void addCar(double angle, sf::Vector2f center);

	void addWall(Segment segment);

	void addFinishLine(Segment segment);

	bool load();

	bool save();

	void clear();

	DrawableManager* getDrawableIntersectionManager();

	DrawableCar* getDrawableCar();
};