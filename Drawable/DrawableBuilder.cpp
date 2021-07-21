#include "DrawableBuilder.hpp"
#include "DrawableWallManager.hpp"
#include "DrawableFinishLine.hpp"
#include <fstream>

void DrawableBuilder::addCar(double angle, sf::Vector2f center)
{
	m_car = true;
	m_carCenter = center;
	m_carAngle = angle;
}

void DrawableBuilder::addWall(Segment segment)
{
	m_walls.push_back(segment);
}

void DrawableBuilder::addFinishLine(Segment segment)
{
	m_finishLine = true;
	m_finishLineSegment = segment;
}

bool DrawableBuilder::load()
{
	return false;
}

bool DrawableBuilder::save()
{
	if (m_walls.empty())
		return false;

	if (!m_car)
		return false;

	if (!m_finishLine)
		return false;

	std::ofstream output("output.txt");
	if (!output.is_open())
		return false;

	output << "Car angle: " << m_carAngle << std::endl;
	output << "Car center: " << m_carCenter.x << " " << m_carCenter.y << std::endl;
	output << "Finish line position: " << m_finishLineSegment[0].x << " " << m_finishLineSegment[0].y << " " << m_finishLineSegment[1].x << " " << m_finishLineSegment[1].y << std::endl;
	for (auto& i : m_walls)
		output << "Segment: " << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	return true;
}

void DrawableBuilder::clear()
{
	m_car = false;
	m_finishLine = false;
}

DrawableWallManager* DrawableBuilder::getDrawableWallManager()
{
	if (m_walls.empty())
		return nullptr;

	DrawableWallManager* result = new DrawableWallManager;
	return result;
}

DrawableCar* DrawableBuilder::getDrawableCar()
{
	if (!m_car)
		return nullptr;

	DrawableCar* result = new DrawableCar(m_carAngle, m_carCenter);
	return result;
}

DrawableFinishLine* DrawableBuilder::getFinishLine()
{
	if (!m_finishLine)
		return nullptr;

	DrawableFinishLine* result = new DrawableFinishLine();
	result->setStartPoint(m_finishLineSegment[0]);
	result->setEndPoint(m_finishLineSegment[1]);
	return result;
}