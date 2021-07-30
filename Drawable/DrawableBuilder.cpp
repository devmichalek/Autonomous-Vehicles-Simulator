#include "DrawableBuilder.hpp"
#include "DrawableManager.hpp"
#include "DrawableFinishLine.hpp"
#include "DrawableCheckpointMap.hpp"
#include "DrawableCheckpointMapRA.hpp"
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

bool DrawableBuilder::getEdgeFromString(std::string line, Edge& result)
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

void DrawableBuilder::addCar(double angle, sf::Vector2f center)
{
	m_carSpecified = true;
	m_carCenter = center;
	m_carAngle = angle;
}

void DrawableBuilder::addEdge(Edge edge)
{
	m_edgeSpecified = true;
	m_edges.push_back(edge);
}

void DrawableBuilder::addFinishLine(Edge edge)
{
	m_finishLineSpecified = true;
	m_finishLineEdge = edge;
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
	if (!getEdgeFromString(line, m_finishLineEdge))
		return false;
	m_finishLineSpecified = true;

	// Get edges
	Edge edge;
	while (std::getline(output, line))
	{
		if (line.find(m_edgeString) != 0)
			return false;

		line.erase(0, m_edgeString.size());
		if (!getEdgeFromString(line, edge))
			return false;
		m_edges.push_back(edge);
	}
	m_edgeSpecified = true;

	return true;
}

bool DrawableBuilder::save(const char* filename)
{
	if (!filename)
		return false;

	if (!m_edgeSpecified)
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
	output << m_finishLineString << m_finishLineEdge[0].x << " " << m_finishLineEdge[0].y << " " << m_finishLineEdge[1].x << " " << m_finishLineEdge[1].y << std::endl;
	
	// Save edges
	for (auto& i : m_edges)
		output << m_edgeString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	clear();
	return true;
}

void DrawableBuilder::clear()
{
	m_edgeSpecified = false;
	m_carSpecified = false;
	m_finishLineSpecified = false;
}

DrawableManager* DrawableBuilder::getDrawableManager()
{
	if(!m_finishLineSpecified)
		return nullptr;

	if (!m_edgeSpecified)
		return nullptr;

	DrawableManager* result = new DrawableManager(std::move(m_edges), std::move(m_finishLineEdge));
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

DrawableCheckpointMap* DrawableBuilder::getDrawableCheckpointMap()
{
	Edge edge = m_edges.front();
	std::vector<size_t> result;
	for (size_t i = 1; i < m_edges.size(); ++i)
	{
		if (m_edges[i - 1][1] == m_edges[i][0])
		{
			// If the end of the previous edge is the beggining of the current edge
			// It meanse there is no gap
			if (edge[0] == m_edges[i][1])
			{
				result.push_back(i + 1);
				if (i + 1 >= m_edges.size())
					break;
				edge = m_edges[i + 1];
				++i;
			}
		}
	}

	if (result.size() != 2)
		return nullptr;

	auto pivot = result.front();
	for (size_t i = 0; i < pivot; ++i)
	{
		for (size_t j = pivot; j < m_edges.size(); ++j)
		{
			if (Intersect(m_edges[i], m_edges[j]))
				return nullptr;
		}
	}

	return new DrawableCheckpointMapRA(m_edges, pivot, m_finishLineEdge);
}