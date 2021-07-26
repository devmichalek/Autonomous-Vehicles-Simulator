#include "DrawableBuilder.hpp"
#include "DrawableManager.hpp"
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

bool DrawableBuilder::getWallFromString(std::string line, Wall& result)
{
	std::vector<float> data;
	while (data.size() != 3)
	{
		size_t position = line.find(' ');
		if (position == std::string::npos)
			break;
		std::string substr = line.substr(0, position);
		line.erase(0, position + 1);
		float coordinate = static_cast<float>(std::atof(substr.c_str()));
		data.push_back(coordinate);
	}
	if (data.size() != 3)
		return false;
	float coordinate = static_cast<float>(std::atof(line.c_str()));
	data.push_back(coordinate);
	result[0].x = data[0];
	result[0].y = data[1];
	result[1].x = data[2];
	result[1].y = data[3];
	return true;
}

void DrawableBuilder::generateCheckpoints()
{

}

void DrawableBuilder::addCar(double angle, sf::Vector2f center)
{
	m_carSpecified = true;
	m_carCenter = center;
	m_carAngle = angle;
}

void DrawableBuilder::addWall(Wall wall)
{
	m_wallSpecified = true;
	m_walls.push_back(wall);
}

void DrawableBuilder::addFinishLine(Wall wall)
{
	m_finishLineSpecified = true;
	m_finishLineSegment = wall;
}

void DrawableBuilder::addCheckpoint(Wall wall)
{
	m_checkpoints.push_back(wall);
}

bool DrawableBuilder::load(const char* filename)
{
	clear();

	if (!filename)
		return false;

	std::ifstream output(filename);
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
	m_carSpecified = true;

	// Get finish line coordinates
	std::getline(output, line);
	if (line.find(m_finishLineString) != 0)
		return false;
	line.erase(0, m_finishLineString.size());
	if (!getWallFromString(line, m_finishLineSegment))
		return false;
	m_finishLineSpecified = true;

	// Get walls
	Wall wall;
	bool checkpointSpecified = false;
	while (std::getline(output, line))
	{
		if (line.find(m_wallString) != 0)
		{
			if (line.find(m_checkpointString) != 0)
				return false;

			// Get checkpoints
			do
			{
				line.erase(0, m_checkpointString.size());
				if (!getWallFromString(line, wall))
					return false;
				m_checkpoints.push_back(wall);
			} while (std::getline(output, line));

			checkpointSpecified = true;
			break;
		}

		line.erase(0, m_wallString.size());
		if (!getWallFromString(line, wall))
			return false;
		m_walls.push_back(wall);
	}
	m_wallSpecified = true;

	if (!checkpointSpecified)
	{
		generateCheckpoints();
	}

	return true;
}

bool DrawableBuilder::save(const char* filename)
{
	if (!filename)
		return false;

	if (!m_wallSpecified)
		return false;

	if (!m_carSpecified)
		return false;

	if (!m_finishLineSpecified)
		return false;

	std::ofstream output(filename);
	if (!output.is_open())
		return false;

	// Save car
	output << m_carAngleString << m_carAngle << std::endl;
	output << m_carCenterString << m_carCenter.x << " " << m_carCenter.y << std::endl;
	
	// Save finish line
	output << m_finishLineString << m_finishLineSegment[0].x << " " << m_finishLineSegment[0].y << " " << m_finishLineSegment[1].x << " " << m_finishLineSegment[1].y << std::endl;
	
	// Save walls
	for (auto& i : m_walls)
		output << m_wallString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	// Save optional checkpoints
	for (auto& i : m_checkpoints)
		output << m_checkpointString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	clear();
	return true;
}

void DrawableBuilder::clear()
{
	m_wallSpecified = false;
	m_carSpecified = false;
	m_finishLineSpecified = false;
}

DrawableManager* DrawableBuilder::getDrawableManager()
{
	if(!m_finishLineSpecified)
		return nullptr;

	if (!m_wallSpecified)
		return nullptr;

	DrawableManager* result = new DrawableManager(std::move(m_walls), std::move(m_finishLineSegment));
	return result;
}

DrawableCar* DrawableBuilder::getDrawableCar()
{
	if (!m_carSpecified)
		return nullptr;

	DrawableCar* result = new DrawableCar;
	result->setAngle(m_carAngle);
	result->setCenter(m_carCenter);
	return result;
}
