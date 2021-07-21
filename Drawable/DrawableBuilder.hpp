#pragma once
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"

class DrawableWallManager;
class DrawableCar;
class DrawableFinishLine;

class DrawableBuilder
{
	bool m_car;
	sf::Vector2f m_carCenter;
	double m_carAngle;

	std::vector<Segment> m_walls;

	bool m_finishLine;
	Segment m_finishLineSegment;

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

	DrawableWallManager* getDrawableWallManager();

	DrawableCar* getDrawableCar();

	DrawableFinishLine* getFinishLine();
};