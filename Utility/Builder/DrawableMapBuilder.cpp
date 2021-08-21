#include "DrawableMapBuilder.hpp"
#include "DrawableEdgeManager.hpp"
#include "DrawableCheckpointMap.hpp"
#include <fstream>

DrawableMapBuilder::DrawableMapBuilder()
{
	Clear();

	m_operationsMap[UNKNOWN] = "Error: last status is unknown!";
	m_operationsMap[SUCCESS_LOAD_COMPLETED] = "Success: correctly opened file!";
	m_operationsMap[SUCCESS_SAVE_COMPLETED] = "Success: correctly saved file!";
	m_operationsMap[SUCCESS_VALIDATION_PASSED] = "Success: validation process has passed with no errors!";
	m_operationsMap[ERROR_VEHICLE_IS_NOT_SPECIFIED] = "Error: vehicle is not specified!";
	m_operationsMap[ERROR_EDGES_ARE_NOT_SPECIFIED] = "Error: edges are not specified!";
	m_operationsMap[ERROR_INCORRECT_EDGE_SEQUENCE_COUNT] = "Error: there should be only two edge sequences!";
	m_operationsMap[ERROR_EDGE_SEQUENCE_INTERSECTION] = "Error: found intersection between edge sequences!";
	m_operationsMap[ERROR_TOO_LITTLE_INNER_EDGES] = "Error: too little inner edges specified!";
	m_operationsMap[ERROR_TOO_LITTLE_OUTER_EDGES] = "Error: too little outer edges specified!";
	m_operationsMap[ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING] = "Error: filename is empty, cannot open file for reading!";
	m_operationsMap[ERROR_CANNOT_OPEN_FILE_FOR_READING] = "Error: cannot open file for reading!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_DOUBLE_WHILE_READING] = "Error: cannot extract floating point value from string while reading!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_POINT_WHILE_READING] = "Error: cannot extract point from string while reading!";
	m_operationsMap[ERROR_CANNOT_EXTRACT_EDGE_WHILE_READING] = "Error: cannot extract edge from string while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_VEHICLE_ANGLE_STRING_WHILE_READING] = "Error: cannot extract vehicle angle string line while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_VEHICLE_CENTER_STRING_WHILE_READING] = "Error: cannot extract vehicle center string line while reading!";
	m_operationsMap[ERROR_CANNOT_FIND_EDGE_STRING_WHILE_READING] = "Error: cannot extract edge string line while reading!";
	m_operationsMap[ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING] = "Error: filename is empty, cannot open file for writing!";
	m_operationsMap[ERROR_CANNOT_OPEN_FILE_FOR_WRITING] = "Error: cannot open file for writing!";
}

DrawableMapBuilder::~DrawableMapBuilder()
{
	Clear();
}

void DrawableMapBuilder::Clear()
{
	m_edgesPivot = 0;
	m_edges.clear();
	m_vehicleSpecified = false;
	m_validated = false;
	m_lastOperationStatus = UNKNOWN;
}

bool DrawableMapBuilder::Validate()
{
	if (m_validated)
	{
		m_lastOperationStatus = SUCCESS_VALIDATION_PASSED;
		return true;
	}

	if (!m_vehicleSpecified)
	{
		m_lastOperationStatus = ERROR_VEHICLE_IS_NOT_SPECIFIED;
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

	if (m_edgesPivot < m_minNumberOfInnerEdges)
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_INNER_EDGES;
		return false;
	}

	if (m_edges.size() - m_edgesPivot < m_maxNumberOfInnerEdges)
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_OUTER_EDGES;
		return false;
	}

	m_lastOperationStatus = SUCCESS_VALIDATION_PASSED;
	m_validated = true;
	return true;
}

void DrawableMapBuilder::AddVehicle(double angle, sf::Vector2f center)
{
	m_vehicleSpecified = true;
	m_vehicleCenter = center;
	m_vehicleAngle = angle;
}

void DrawableMapBuilder::AddEdge(Edge edge)
{
	m_edges.push_back(edge);
}

bool DrawableMapBuilder::Load(std::string filename)
{
	Clear();

	// Check if filename is not empty
	if (filename.empty())
	{
		m_lastOperationStatus = ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_READING;
		return false;
	}

	// Check if file can be opened for reading
	std::ifstream input(filename);
	if (!input.is_open())
	{
		m_lastOperationStatus = ERROR_CANNOT_OPEN_FILE_FOR_READING;
		return false;
	}

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
	m_vehicleSpecified = true;

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

	m_lastOperationStatus = SUCCESS_LOAD_COMPLETED;
	return true;
}

bool DrawableMapBuilder::Save(std::string filename)
{
	// Check if filename is not empty
	if (filename.empty())
	{
		m_lastOperationStatus = ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Validate
	if (!Validate())
		return false;

	// Check if file can be opened for writing
	std::ofstream output(filename);
	if (!output.is_open())
	{
		m_lastOperationStatus = ERROR_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Save vehicle
	output << m_vehicleAngleString << m_vehicleAngle << std::endl;
	output << m_vehicleCenterString << m_vehicleCenter.x << " " << m_vehicleCenter.y << std::endl;
	
	// Save edges
	for (auto& i : m_edges)
		output << m_edgeString << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;

	Clear();

	m_lastOperationStatus = SUCCESS_SAVE_COMPLETED;
	return true;
}

std::pair<bool, std::string> DrawableMapBuilder::GetLastOperationStatus()
{
	std::string message = m_operationsMap[m_lastOperationStatus];
	switch (m_lastOperationStatus)
	{
		case SUCCESS_SAVE_COMPLETED:
		case SUCCESS_LOAD_COMPLETED:
		case SUCCESS_VALIDATION_PASSED:
			return std::make_pair(true, message);
		default:
			return std::make_pair(false, message);
	}

	return std::make_pair(false, message);
}

EdgeVector DrawableMapBuilder::GetEdges()
{
	return m_edges;
}

std::pair<sf::Vector2f, double> DrawableMapBuilder::GetVehicle()
{
	return std::make_pair(m_vehicleCenter, m_vehicleAngle);
}

DrawableEdgeManager* DrawableMapBuilder::GetDrawableManager()
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