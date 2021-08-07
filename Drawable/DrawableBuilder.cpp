#include "DrawableBuilder.hpp"
#include "DrawableEdgeManager.hpp"
#include "DrawableCheckpointMap.hpp"
#include "DrawableCheckpointMapBeam.hpp"
#include "DrawableCheckpointMapTriangle.hpp"
#include <fstream>

DrawableBuilder::DrawableBuilder()
{
	Clear();
}

bool DrawableBuilder::GetPointFromString(std::string line, sf::Vector2f& result)
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

bool DrawableBuilder::GetEdgeFromString(std::string line, Edge& result)
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

std::vector<size_t> DrawableBuilder::CountEdgeSequences()
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

bool DrawableBuilder::IsEdgeSequenceIntersection(std::vector<size_t> pivots)
{
	auto pivot = pivots.front();
	for (size_t i = 0; i < pivot; ++i)
	{
		for (size_t j = pivot; j < m_edges.size(); ++j)
		{
			if (Intersect(m_edges[i], m_edges[j]))
				return true;
		}
	}

	return false;
}

bool DrawableBuilder::ValidateEdges()
{
	if (m_validated)
		return true;

	if (m_edges.empty())
		return false;

	auto pivots = CountEdgeSequences();
	if (pivots.size() != 2)
		return false;

	if (IsEdgeSequenceIntersection(pivots))
		return false;

	// Set number of inner edges, pivot from where outer edges start
	m_edgesPivot = pivots.front();

	if (m_edgesPivot < 4)
		return false;

	if (m_edges.size() - m_edgesPivot < 4)
		return false;

	// Add finish line position
	Edge finishLineEdge;
	finishLineEdge[0] = m_edges.front()[0];
	finishLineEdge[1] = m_edges[m_edgesPivot][0];
	m_edges.push_back(finishLineEdge);

	// Set blocking edge position
	Edge blockEdge;
	blockEdge[0] = m_edges.front()[1];
	blockEdge[1] = m_edges[m_edgesPivot][1];
	m_edges.insert(m_edges.begin(), blockEdge);
	++m_edgesPivot;

	/*m_edges.erase(m_edges.begin() + m_edgesPivot);
	m_edges.erase(m_edges.begin() + 1);
	--m_edgesPivot;*/

	m_validated = true;
	return true;
}

void DrawableBuilder::AddCar(double angle, sf::Vector2f center)
{
	m_carSpecified = true;
	m_carCenter = center;
	m_carAngle = angle;
}

void DrawableBuilder::AddEdge(Edge edge)
{
	m_edges.push_back(edge);
}

bool DrawableBuilder::Load(const char* filename)
{
	Clear();

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
	if (!GetPointFromString(line, m_carCenter))
		return false;
	m_carSpecified = true;

	// Get edges
	Edge edge;
	while (std::getline(output, line))
	{
		if (line.find(m_edgeString) != 0)
			return false;

		line.erase(0, m_edgeString.size());
		if (!GetEdgeFromString(line, edge))
			return false;
		m_edges.push_back(edge);
	}

	if (!ValidateEdges())
	{
		Clear();
		return false;
	}

	return true;
}

bool DrawableBuilder::Save(const char* filename)
{
	Clear();

	if (!filename)
		return false;

	if (!ValidateEdges())
		return false;

	if (!m_carSpecified)
		return false;

	std::ofstream output(filename);
	if (!output.is_open())
		return false;

	// Save car
	output << m_carAngleString << m_carAngle << std::endl;
	output << m_carCenterString << m_carCenter.x << " " << m_carCenter.y << std::endl;
	
	// Save edges
	for (auto& i : m_edges)
		output << m_edgeString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	return true;
}

void DrawableBuilder::Clear()
{
	m_edgesPivot = 0;
	m_edges.clear();
	m_carSpecified = false;
	m_validated = false;
}

DrawableEdgeManager* DrawableBuilder::GetDrawableManager()
{
	if (!ValidateEdges())
		return nullptr;

	DrawableEdgeManager* result = new DrawableEdgeManager(std::move(m_edges));
	return result;
}

DrawableCar* DrawableBuilder::GetDrawableCar()
{
	if (!m_carSpecified)
		return nullptr;

	DrawableCar* result = new DrawableCar;
	result->setAngle(m_carAngle);
	result->setCenter(m_carCenter);
	return result;
}

DrawableCheckpointMap* DrawableBuilder::GetDrawableCheckpointMap()
{
	if (!ValidateEdges())
		return nullptr;

	//return new DrawableCheckpointMapBeam(m_edges, m_edgesPivot);
	return new DrawableCheckpointMapTriangle(m_edges, m_edgesPivot);
}