#include "DrawableBuilder.hpp"
#include "DrawableWallManager.hpp"
#include "DrawableFinishLine.hpp"
#include <fstream>

bool DrawableBuilder::getPointFromString(std::string line, sf::Vector2f& result)
{
	size_t position = line.find(' ');
	if (position == std::string::npos)
		return false;

	std::string xString = line.substr(0, position);
	line.erase(0, position + 1);
	float x = (float)std::atof(xString.c_str());
	float y = (float)std::atof(line.c_str());
	result = sf::Vector2f(x, y);
	return true;
}

bool DrawableBuilder::getSegmentFromString(std::string line, Segment& result)
{
	std::vector<float> data;
	while (data.size() != 3)
	{
		size_t position = line.find(' ');
		if (position == std::string::npos)
			break;
		std::string substr = line.substr(0, position);
		line.erase(0, position + 1);
		float coordinate = std::atof(substr.c_str());
		data.push_back(coordinate);
	}
	if (data.size() != 3)
		return false;
	float coordinate = std::atof(line.c_str());
	data.push_back(coordinate);
	result[0].x = data[0];
	result[0].y = data[1];
	result[1].x = data[2];
	result[1].y = data[3];
	return true;
}

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
	std::ifstream output(m_inputFilename);
	if (!output.is_open())
		return false;

	// Get car angle
	std::string line;
	std::getline(output, line);
	if (line.find(m_carAngleString) != 0)
		return false;
	line.erase(0, m_carAngleString.size());
	m_carAngle = std::atof(line.c_str());
	
	// Get car center
	std::getline(output, line);
	if (line.find(m_carCenterString) != 0)
		return false;
	line.erase(0, m_carCenterString.size());
	if (!getPointFromString(line, m_carCenter))
		return false;
	m_car = true;

	// Get finish line coordinates
	std::getline(output, line);
	if (line.find(m_finishLineString) != 0)
		return false;
	line.erase(0, m_finishLineString.size());
	if (!getSegmentFromString(line, m_finishLineSegment))
		return false;
	m_finishLine = true;

	// Get walls
	while (std::getline(output, line))
	{
		if (line.find(m_wallString) != 0)
			return false;
		line.erase(0, m_wallString.size());
		Segment segment;
		if (!getSegmentFromString(line, segment))
			return false;
		m_walls.push_back(segment);
	}

	return true;
}

bool DrawableBuilder::save()
{
	if (m_walls.empty())
		return false;

	if (!m_car)
		return false;

	if (!m_finishLine)
		return false;

	std::ofstream output(m_outputFilename);
	if (!output.is_open())
		return false;

	output << m_carAngleString << m_carAngle << std::endl;
	output << m_carCenterString << m_carCenter.x << " " << m_carCenter.y << std::endl;
	output << m_finishLineString << m_finishLineSegment[0].x << " " << m_finishLineSegment[0].y << " " << m_finishLineSegment[1].x << " " << m_finishLineSegment[1].y << std::endl;
	for (auto& i : m_walls)
		output << m_wallString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

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

	DrawableWallManager* result = new DrawableWallManager(std::move(m_walls));
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