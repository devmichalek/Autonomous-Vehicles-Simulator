#include "DrawableMapBuilder.hpp"
#include "DrawableEdgeManager.hpp"
#include "DrawableCheckpointMap.hpp"

bool DrawableMapBuilder::ValidateInternal()
{
	if (!m_vehiclePositioned)
	{
		m_lastOperationStatus = ERROR_VEHICLE_IS_NOT_POSITIONED;
		return false;
	}

	if (m_edges.empty())
	{
		m_lastOperationStatus = ERROR_EDGES_ARE_NOT_SPECIFIED;
		return false;
	}

	auto CountEdgeSequences = [&]() {
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
	};

	auto pivots = CountEdgeSequences();
	if (pivots.size() != 2)
	{
		m_lastOperationStatus = ERROR_INCORRECT_EDGE_SEQUENCE_COUNT;
		return false;
	}

	auto IsEdgeSequenceIntersection = [&]()
	{
		auto pivot = pivots.front();
		for (size_t i = 0; i < pivot; ++i)
		{
			for (size_t j = pivot; j < m_edges.size(); ++j)
			{
				if (DrawableMath::Intersect(m_edges[i], m_edges[j]))
					return true;
			}
		}

		return false;
	};

	if (IsEdgeSequenceIntersection())
	{
		m_lastOperationStatus = ERROR_EDGE_SEQUENCE_INTERSECTION;
		return false;
	}

	// Set number of inner edges, pivot from where outer edges start
	m_edgesPivot = pivots.front();

	if (m_edgesPivot < GetMinNumberOfInnerEdges())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_INNER_EDGES;
		return false;
	}

	if (m_edges.size() - m_edgesPivot < GetMaxNumberOfInnerEdges())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_OUTER_EDGES;
		return false;
	}

	return true;
}

void DrawableMapBuilder::ClearInternal()
{
	m_edgesPivot = 0;
	m_edges.clear();
	m_vehiclePositioned = false;
}

bool DrawableMapBuilder::LoadInternal(std::ifstream& input)
{
	auto ExtractDoubleFromString = [&](std::string input, double& output)
	{
		if (input.empty())
		{
			m_lastOperationStatus = ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING;
			return false;
		}

		const char* character = &input[0];
		if (*character == '-' || *character == '+')
			++character;

		size_t periodCount = 0;
		while (character && *character)
		{
			if (*character == '.')
			{
				++periodCount;
				if (periodCount > 1)
				{
					m_lastOperationStatus = ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING;
					return false;
				}
			}
			else if (*character < '0' || *character > '9')
			{
				m_lastOperationStatus = ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING;
				return false;
			}

			++character;
		}

		output = std::atof(input.c_str());
		return true;
	};

	auto ExtractPointFromString = [&](std::string line, sf::Vector2f& result)
	{
		size_t position = line.find(' ');
		if (position == std::string::npos)
		{
			m_lastOperationStatus = ERROR_CANNOT_EXTRACT_POINT_WHILE_READING;
			return false;
		}

		double x;
		std::string xString = line.substr(0, position);
		if (!ExtractDoubleFromString(xString, x))
			return false;

		double y;
		std::string yString = line.erase(0, position + 1);
		if (!ExtractDoubleFromString(yString, y))
			return false;

		result = sf::Vector2f(float(x), float(y));
		return true;
	};

	auto ExtractEdgeFromString = [&](std::string line, Edge& result)
	{
		std::vector<float> data;
		while (data.size() != 3)
		{
			size_t position = line.find(' ');
			if (position == std::string::npos)
				break;

			double coordinate;
			std::string substr = line.substr(0, position);
			if (!ExtractDoubleFromString(substr, coordinate))
				return false;

			data.push_back(float(coordinate));
			line.erase(0, position + 1);
		}

		if (data.size() != 3)
		{
			m_lastOperationStatus = ERROR_CANNOT_EXTRACT_EDGE_WHILE_READING;
			return false;
		}

		float coordinate = static_cast<float>(std::atof(line.c_str()));
		data.push_back(coordinate);
		result[0].x = data[0];
		result[0].y = data[1];
		result[1].x = data[2];
		result[1].y = data[3];
		return true;
	};

	// Get vehicle angle
	std::string line;
	std::getline(input, line);
	if (line.find(m_vehicleAngleString) != 0)
	{
		m_lastOperationStatus = ERROR_CANNOT_FIND_VEHICLE_ANGLE_STRING_WHILE_READING;
		return false;
	}
	line.erase(0, m_vehicleAngleString.size());
	if (!ExtractDoubleFromString(line, m_vehicleAngle))
		return false;

	// Get vehicle center
	std::getline(input, line);
	if (line.find(m_vehicleCenterString) != 0)
	{
		m_lastOperationStatus = ERROR_CANNOT_FIND_VEHICLE_CENTER_STRING_WHILE_READING;
		return false;
	}
	line.erase(0, m_vehicleCenterString.size());
	if (!ExtractPointFromString(line, m_vehicleCenter))
		return false;
	m_vehiclePositioned = true;

	// Get edges
	Edge edge;
	while (std::getline(input, line))
	{
		if (line.find(m_edgeString) != 0)
		{
			m_lastOperationStatus = ERROR_CANNOT_FIND_EDGE_STRING_WHILE_READING;
			return false;
		}

		line.erase(0, m_edgeString.size());
		if (!ExtractEdgeFromString(line, edge))
			return false;
		m_edges.push_back(edge);
	}

	if (!Validate())
		return false;

	return true;
}

bool DrawableMapBuilder::SaveInternal(std::ofstream& output)
{
	// Save vehicle
	output << m_vehicleAngleString << m_vehicleAngle << std::endl;
	output << m_vehicleCenterString << m_vehicleCenter.x << " " << m_vehicleCenter.y << std::endl;

	// Save edges
	for (auto& i : m_edges)
		output << m_edgeString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	return true;
}

void DrawableMapBuilder::CreateDummyInternal()
{
	auto windowSize = CoreWindow::GetSize();
	float xOffset = windowSize.x / 5.0f;
	float yOffset = windowSize.y / 5.0f;

	auto innerPoint1 = sf::Vector2f(xOffset * 2, yOffset * 2);
	auto innerPoint2 = sf::Vector2f(xOffset * 3, innerPoint1.y);
	auto innerPoint3 = sf::Vector2f(innerPoint2.x, yOffset * 3);
	auto innerPoint4 = sf::Vector2f(innerPoint1.x, innerPoint3.y);

	auto outerPoint1 = sf::Vector2f(xOffset, yOffset);
	auto outerPoint2 = sf::Vector2f(xOffset * 4, outerPoint1.y);
	auto outerPoint3 = sf::Vector2f(outerPoint2.x, yOffset * 4);
	auto outerPoint4 = sf::Vector2f(outerPoint1.x, outerPoint3.y);

	m_edges.push_back({ innerPoint1, innerPoint2 });
	m_edges.push_back({ innerPoint2, innerPoint3 });
	m_edges.push_back({ innerPoint3, innerPoint4 });
	m_edges.push_back({ innerPoint4, innerPoint1 });

	m_edges.push_back({ outerPoint1, outerPoint2 });
	m_edges.push_back({ outerPoint2, outerPoint3 });
	m_edges.push_back({ outerPoint3, outerPoint4 });
	m_edges.push_back({ outerPoint4, outerPoint1 });

	m_edgesPivot = 4;
	m_vehiclePositioned = true;
	m_vehicleCenter = sf::Vector2f(xOffset * 1.5, yOffset * 2.5);
	m_vehicleAngle = 270.0;
}

DrawableMapBuilder::DrawableMapBuilder() :
	AbstractBuilder(std::ios::in, std::ios::out),
	m_edgesPivot(0),
	m_vehiclePositioned(false),
	m_vehicleAngle(0.0)
{
	m_operationsMap[ERROR_VEHICLE_IS_NOT_POSITIONED] = "Error: vehicle is not positioned!";
	m_operationsMap[ERROR_EDGES_ARE_NOT_SPECIFIED] = "Error: edges are not specified!";
	m_operationsMap[ERROR_INCORRECT_EDGE_SEQUENCE_COUNT] = "Error: there should be only two edge sequences!";
	m_operationsMap[ERROR_EDGE_SEQUENCE_INTERSECTION] = "Error: found intersection between edge sequences!";
	m_operationsMap[ERROR_TOO_LITTLE_INNER_EDGES] = "Error: too little inner edges specified!";
	m_operationsMap[ERROR_TOO_LITTLE_OUTER_EDGES] = "Error: too little outer edges specified!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING] = "Error: cannot extract floating point value from string while reading!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_POINT_WHILE_READING] = "Error: cannot extract point from string while reading!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_EDGE_WHILE_READING] = "Error: cannot extract edge from string while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_VEHICLE_ANGLE_STRING_WHILE_READING] = "Error: cannot extract vehicle angle string line while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_VEHICLE_CENTER_STRING_WHILE_READING] = "Error: cannot extract vehicle center string line while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_EDGE_STRING_WHILE_READING] = "Error: cannot extract edge string line while reading!";
	Clear();
}

DrawableMapBuilder::~DrawableMapBuilder()
{
}

void DrawableMapBuilder::AddVehicle(double angle, sf::Vector2f center)
{
	m_vehiclePositioned = true;
	m_vehicleCenter = center;
	m_vehicleAngle = angle;
}

void DrawableMapBuilder::AddEdge(Edge edge)
{
	m_edges.push_back(edge);
}

EdgeVector DrawableMapBuilder::GetEdges()
{
	return m_edges;
}

std::pair<sf::Vector2f, double> DrawableMapBuilder::GetVehicle()
{
	return std::make_pair(m_vehicleCenter, m_vehicleAngle);
}

DrawableEdgeManager* DrawableMapBuilder::GetDrawableMap()
{
	if (!Validate())
		return nullptr;

	return new DrawableEdgeManager(m_edges, m_edgesPivot);
}

DrawableCheckpointMap* DrawableMapBuilder::GetDrawableCheckpointMap()
{
	if (!Validate())
		return nullptr;
	
	return new DrawableCheckpointMap(m_edges, m_edgesPivot);
}

size_t DrawableMapBuilder::GetMinNumberOfInnerEdges() const
{
	return 4;
}

size_t DrawableMapBuilder::GetMaxNumberOfInnerEdges() const
{
	return GetMinNumberOfInnerEdges();
}
