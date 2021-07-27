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

std::vector<size_t> DrawableBuilder::getEdgeSequences()
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

	return result;
}

bool DrawableBuilder::isOverlappingEdgeSequence(size_t iMin, size_t iMax, size_t jMin, size_t jMax)
{
	for (size_t i = iMin; i < iMax; ++i)
	{
		for (size_t j = jMin; j < jMax; ++j)
		{
			if (Intersect(m_edges[i], m_edges[j]))
				return true;
		}
	}

	return false;
}

void DrawableBuilder::generateCheckpoints(size_t iMin, size_t iMax, size_t jMin, size_t jMax)
{
	const float checkpointLength = float(CoreWindow::getSize().x);
	const float checkpointGap = float((CoreWindow::getSize().x / 30.0f) / 3);

	// For all inner edges from the edge sequence
	for (size_t i = iMin; i < iMax; ++i)
	{
		auto edgeLength = Distance(m_edges[i]) - 1;
		auto p1 = m_edges[i][0];
		auto p2 = m_edges[i][1];
		float angle = (float)Angle(p1, p2);

		const float checkpointAngle = angle + 90; // 90 degrees
		size_t checkpointCount = static_cast<size_t>(edgeLength / checkpointGap) + 1;

		for (float gap = 0; checkpointCount-- > 0; gap -= checkpointGap)
		{
			sf::Vector2f startPoint = GetEndPoint(p1, angle, gap);
			startPoint = GetEndPoint(startPoint, checkpointAngle, 1.0);
			sf::Vector2f endPoint = GetEndPoint(startPoint, checkpointAngle, checkpointLength);
			m_innerCheckpoints.push_back({ startPoint, endPoint });
		}
	}

	// For all outer edges from the edge sequence
	for (size_t i = jMin; i < jMax; ++i)
	{
		auto edgeLength = Distance(m_edges[i]) - 1;
		auto p1 = m_edges[i][0];
		auto p2 = m_edges[i][1];
		float angle = (float)Angle(p1, p2);

		const float checkpointAngle = angle - 90; // 90 degrees
		size_t checkpointCount = static_cast<size_t>(edgeLength / checkpointGap) + 1;

		for (float gap = 0; checkpointCount-- > 0; gap -= checkpointGap)
		{
			sf::Vector2f startPoint = GetEndPoint(p1, angle, gap);
			startPoint = GetEndPoint(startPoint, checkpointAngle, 1.0);
			sf::Vector2f endPoint = GetEndPoint(startPoint, checkpointAngle, checkpointLength);
			m_outerCheckpoints.push_back({ startPoint, endPoint });
		}
	}

	// Check if particular inner checkpoint does not intersect with a edge
	// If there is intersection then shorten the checkpoint
	sf::Vector2f intersectionPoint;
	for (auto &i : m_innerCheckpoints)
	{
		for (auto &j : m_edges)
		{
			if (GetIntersectionPoint(i, j, intersectionPoint))
			{
				i[1] = intersectionPoint;
			}
		}

		if (GetIntersectionPoint(i, m_finishLineSegment, intersectionPoint))
		{
			i[1] = intersectionPoint;
		}
	}

	// Check if particular outer checkpoint does not intersect with a edge
	// If there is intersection then shorten the checkpoint
	for (auto& i : m_outerCheckpoints)
	{
		for (auto& j : m_edges)
		{
			if (GetIntersectionPoint(i, j, intersectionPoint))
			{
				i[1] = intersectionPoint;
			}
		}

		if (GetIntersectionPoint(i, m_finishLineSegment, intersectionPoint))
		{
			i[1] = intersectionPoint;
		}
	}
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
	m_finishLineSegment = edge;
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
	if (!getEdgeFromString(line, m_finishLineSegment))
		return false;
	m_finishLineSpecified = true;

	// Get edges
	Edge edge;
	bool checkpointSpecified = false;
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

	if (!checkpointSpecified)
	{
		auto lengths = getEdgeSequences();
		if (lengths.size() != 2)
			return false;

		auto i = lengths.front();
		if (isOverlappingEdgeSequence(0, i, i, m_edges.size()))
			return false;

		generateCheckpoints(0, i, i, m_edges.size());
	}

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
	output << m_finishLineString << m_finishLineSegment[0].x << " " << m_finishLineSegment[0].y << " " << m_finishLineSegment[1].x << " " << m_finishLineSegment[1].y << std::endl;
	
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

	DrawableManager* result = new DrawableManager(std::move(m_edges),
												  std::move(m_finishLineSegment),
												  std::move(m_innerCheckpoints),
												  std::move(m_outerCheckpoints));
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
