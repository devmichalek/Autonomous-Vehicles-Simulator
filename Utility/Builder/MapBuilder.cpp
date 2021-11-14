#include "MapBuilder.hpp"
#include "CoreLogger.hpp"
#include "CoreWindow.hpp"

std::pair<sf::Vector2f, sf::Vector2f> MapBuilder::m_maxAllowedMapArea;
std::pair<sf::Vector2f, sf::Vector2f> MapBuilder::m_maxAllowedViewArea;

bool MapBuilder::EdgesChainGenerator::IsClockwiseOrder(const EdgeVector& edges)
{
	float sum = 0.f;
	for (const auto& edge : edges)
	{
		float x = edge[1].x - edge[0].x;
		float y = edge[1].y + edge[0].y;
		sum += (x * y);
	}

	return sum < 0.f;
}

EdgeVector MapBuilder::EdgesChainGenerator::Generate(const EdgeVector& edges, bool innerCollision)
{
	// Determine order
	bool clockwise = IsClockwiseOrder(edges);
	clockwise = innerCollision ? !clockwise : clockwise;
	if (clockwise)
		return edges;

	size_t numberOfEdges = edges.size();
	EdgeVector result(numberOfEdges);
	for (size_t i = 0; i < numberOfEdges; ++i)
	{
		result[i][0] = edges[numberOfEdges - 1 - i][1];
		result[i][1] = edges[numberOfEdges - 1 - i][0];
	}

	return result;
}

EdgeVector MapBuilder::RectangleCheckpointsGenerator::GenerateInternal(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain)
{
	auto length = innerEdgesChain.size();
	EdgeVector result(length);
	for (size_t i = 0; i < length; ++i)
	{
		result[i][0] = innerEdgesChain[i][0];
		result[i][1] = outerEdgesChain[i][0];
	}

	// Validate if there are no intersections with inner edges chain

	auto Validate = [](const EdgeVector& checkpoints, const EdgeVector& edgesChain) {
		const size_t length = checkpoints.size();

		for (size_t i = 0; i < length; ++i)
		{
			for (size_t j = 0; j < length; ++j)
			{
				if (i == j)
					continue;

				if (i == 0)
				{
					if (j == length - 1)
						continue;
				}
				else if (i - 1 == j)
					continue;

				if (DrawableMath::Intersect(checkpoints[i], edgesChain[j]))
					return false;
			}
		}

		return true;
	};

	if (!Validate(result, innerEdgesChain))
		return {};

	if (!Validate(result, outerEdgesChain))
		return {};

	return result;
}

RectangleVector MapBuilder::RectangleCheckpointsGenerator::Generate(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain)
{
	const auto lineCheckpoints = GenerateInternal(innerEdgesChain, outerEdgesChain);
	if (lineCheckpoints.empty())
		return {};

	auto AddRectangleCheckpoints = [](RectangleVector& result, const Edge& beginEdge, const Edge& endEdge) {
		const double minWidth = double(CoreWindow::GetWindowSize().x / 16.f);
		const auto innerDistance = DrawableMath::Distance(beginEdge[0], endEdge[0]);
		const auto outerDistance = DrawableMath::Distance(beginEdge[1], endEdge[1]);
		const size_t innerCount = size_t(innerDistance / minWidth);
		const size_t outerCount = size_t(outerDistance / minWidth);
		const size_t count = (innerCount > outerCount) ? innerCount : outerCount;
		const float innerOffset = float(innerDistance) / float(count);
		const float outerOffset = float(outerDistance) / float(count);

		double innerAngle = DrawableMath::DifferenceVectorAngle(beginEdge[0], endEdge[0]);
		double outerAngle = DrawableMath::DifferenceVectorAngle(beginEdge[1], endEdge[1]);

		Edge previousEdge = beginEdge;
		for (size_t i = 1; i < count; ++i)
		{
			auto p1 = previousEdge[0];
			auto p2 = previousEdge[1];
			auto p3 = DrawableMath::GetEndPoint(beginEdge[1], outerAngle, -outerOffset * i);
			auto p4 = DrawableMath::GetEndPoint(beginEdge[0], innerAngle, -innerOffset * i);
			
			result.push_back({ p1, p2, p3, p4 });
			
			previousEdge[0] = p4;
			previousEdge[1] = p3;
		}

		result.push_back({ previousEdge[0], previousEdge[1], endEdge[1], endEdge[0] });
	};

	// Generate rectangle checkpoints
	RectangleVector result;
	const auto numberOfLineCheckpoints = lineCheckpoints.size();
	for (size_t i = 0; i < numberOfLineCheckpoints - 1; ++i)
		AddRectangleCheckpoints(result, lineCheckpoints[i], lineCheckpoints[i + 1]);
	AddRectangleCheckpoints(result, lineCheckpoints[numberOfLineCheckpoints - 1], lineCheckpoints[0]);

	// Check chepoints against edges


	return result;
}

bool MapBuilder::ValidateMapAreaVehiclePosition()
{
	auto allowedMapArea = GetMaxAllowedMapArea();
	if (!DrawableMath::IsPointInsideRectangle(allowedMapArea.second, allowedMapArea.first, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_MAP_AREA;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateRoadAreaVehiclePosition()
{
	std::vector<sf::Vector2f> innerPoints(m_edgesPivot);
	for (size_t i = 0; i < innerPoints.size(); ++i)
		innerPoints[i] = m_edges[i][0];

	// If vehicle center is inside inner polygon return error
	if (DrawableMath::IsPointInsidePolygon(innerPoints, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_ROAD_AREA;
		return false;
	}

	std::vector<sf::Vector2f> outerPoints(m_edges.size() - m_edgesPivot);
	for (size_t i = 0; i < outerPoints.size(); ++i)
		outerPoints[i] = m_edges[i + m_edgesPivot][0];

	// If vehicle is not inside outer polygon return error
	if (!DrawableMath::IsPointInsidePolygon(outerPoints, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_ROAD_AREA;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateVehicleAngle()
{
	if (m_vehicleAngle < GetMinVehicleAngle())
	{
		m_lastOperationStatus = ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE;
		return false;
	}

	if (m_vehicleAngle > GetMaxVehicleAngle())
	{
		m_lastOperationStatus = ERROR_VEHICLE_ANGLE_IS_TOO_LARGE;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateTotalNumberOfEdges(size_t count)
{
	if (count == 0)
	{
		m_lastOperationStatus = ERROR_EDGES_ARE_NOT_SPECIFIED;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateEdgesPivot(size_t pivot, size_t count)
{
	if (pivot < GetMinNumberOfInnerEdges())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_INNER_EDGES;
		return false;
	}

	if (pivot > GetMaxNumberOfInnerEdges())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_INNER_EDGES;
		return false;
	}

	if (count < pivot)
		count = 0;
	else
		count -= pivot;

	if (count < GetMinNumberOfOuterEdges())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_OUTER_EDGES;
		return false;
	}

	if (count > GetMaxNumberOfOuterEdges())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_OUTER_EDGES;
		return false;
	}

	if (count != pivot)
	{
		m_lastOperationStatus = ERROR_DIFFERENT_NUMBER_OF_INNER_AND_OUTER_EDGES;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateNumberOfEdgeSequences()
{
	Edge edge = m_edges.front();
	std::vector<size_t> result;
	for (size_t i = 1; i < m_edges.size(); ++i)
	{
		if (m_edges[i - 1][1] == m_edges[i][0])
		{
			// If the end of the previous edge is the beggining of the current edge
			// It means that there is no gap
			if (edge[0] == m_edges[i][1])
			{
				result.push_back(i + 1);
				if (i + 1 >= m_edges.size())
					break;
				edge = m_edges[i + 1];
				++i;

				if (i + 1 >= m_edges.size())
				{
					// n edge sequences were found and we are left with one edge alone which is incorrect
					m_lastOperationStatus = ERROR_INCORRECT_EDGE_SEQUENCE_COUNT;
					return false;
				}
			}
		}
		else
		{
			m_lastOperationStatus = ERROR_INCORRECT_EDGE_SEQUENCE_COUNT;
			return false;
		}
	}

	if (result.size() != 2)
	{
		m_lastOperationStatus = ERROR_INCORRECT_EDGE_SEQUENCE_COUNT;
		return false;
	}

	// Set number of inner edges, pivot from where outer edges start
	m_edgesPivot = result.front();

	return true;
}

bool MapBuilder::ValidateEdgeSequenceIntersection()
{
	for (size_t i = 0; i < m_edgesPivot; ++i)
	{
		for (size_t j = m_edgesPivot; j < m_edges.size(); ++j)
		{
			if (DrawableMath::Intersect(m_edges[i], m_edges[j]))
			{
				m_lastOperationStatus = ERROR_EDGE_SEQUENCE_INTERSECTION;
				return false;
			}
		}
	}

	return true;
}

bool MapBuilder::ValidateCheckpoints()
{
	auto innerEdges = EdgeVector(m_edges.begin(), m_edges.begin() + m_edgesPivot);
	auto outerEdges = EdgeVector(m_edges.begin() + m_edgesPivot, m_edges.end());
	auto checkpoints = RectangleCheckpointsGenerator::Generate(innerEdges, outerEdges);
	if (checkpoints.empty())
	{
		m_lastOperationStatus = ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateInternal()
{
	if (!m_vehiclePositioned)
	{
		m_lastOperationStatus = ERROR_VEHICLE_IS_NOT_POSITIONED;
		return false;
	}

	if (!ValidateMapAreaVehiclePosition())
		return false;

	if (!ValidateVehicleAngle())
		return false;

	if (!ValidateTotalNumberOfEdges(m_edges.size()))
		return false;

	if (!ValidateNumberOfEdgeSequences())
		return false;

	if (!ValidateEdgesPivot(m_edgesPivot, m_edges.size()))
		return false;

	if (!ValidateEdgeSequenceIntersection())
		return false;

	if (!ValidateCheckpoints())
		return false;

	if (!ValidateRoadAreaVehiclePosition())
		return false;

	return true;
}

void MapBuilder::ClearInternal()
{
	m_edgesPivot = 0;
	m_edges.clear();
	m_vehiclePositioned = false;
}

bool MapBuilder::LoadInternal(std::ifstream& input)
{
	// Read vehicle center position
	input.read((char*)&m_vehicleCenter.x, sizeof(m_vehicleCenter.x));
	input.read((char*)&m_vehicleCenter.y, sizeof(m_vehicleCenter.y));
	m_vehiclePositioned = true;

	// Validate if vehicle center is inside map area
	if (!ValidateMapAreaVehiclePosition())
		return false;

	// Read vehicle angle
	input.read((char*)&m_vehicleAngle, sizeof(m_vehicleAngle));

	if (!ValidateVehicleAngle())
		return false;

	// Read number of edges
	size_t numberOfEdges = 0;
	input.read((char*)&numberOfEdges, sizeof(numberOfEdges));

	if (!ValidateTotalNumberOfEdges(numberOfEdges))
		return false;

	// Read edges pivot
	input.read((char*)&m_edgesPivot, sizeof(m_edgesPivot));

	if (!ValidateEdgesPivot(m_edgesPivot, numberOfEdges))
		return false;

	// Read edges points
	m_edges.resize(numberOfEdges);
	for (auto& i : m_edges)
	{
		input.read((char*)&i[0].x, sizeof(i[0].x));
		input.read((char*)&i[0].y, sizeof(i[0].y));
	}

	// Pin edges
	for (size_t i = 0; i < m_edgesPivot - 1; i++)
		m_edges[i][1] = m_edges[i + 1][0];
	m_edges[m_edgesPivot - 1][1] = m_edges[0][0];

	for (size_t i = m_edgesPivot; i < numberOfEdges - 1; i++)
		m_edges[i][1] = m_edges[i + 1][0];
	m_edges[numberOfEdges - 1][1] = m_edges[m_edgesPivot][0];

	// Validate edge sequence intersection
	if (!ValidateEdgeSequenceIntersection())
		return false;

	// Validate checkpoints
	if (!ValidateCheckpoints())
		return false;

	// Validate if vehicle center is inside road area
	if (!ValidateRoadAreaVehiclePosition())
		return false;

	return true;
}

bool MapBuilder::SaveInternal(std::ofstream& output)
{
	// Save vehicle center position
	output.write((const char*)&m_vehicleCenter.x, sizeof(m_vehicleCenter.x));
	output.write((const char*)&m_vehicleCenter.y, sizeof(m_vehicleCenter.y));

	// Save vehicle angle
	output.write((const char*)&m_vehicleAngle, sizeof(m_vehicleAngle));
	
	// Save number of edges
	size_t numberOfEdges = m_edges.size();
	output.write((const char*)&numberOfEdges, sizeof(numberOfEdges));

	// Save edges pivot
	output.write((const char*)&m_edgesPivot, sizeof(m_edgesPivot));

	// Save edges beggining position
	for (auto& i : m_edges)
	{
		output.write((const char*)&i[0].x, sizeof(i[0].x));
		output.write((const char*)&i[0].y, sizeof(i[0].y));
	}

	return true;
}

void MapBuilder::CreateDummyInternal()
{
	auto windowSize = CoreWindow::GetWindowSize();
	float xOffset = windowSize.x / 5.0f;
	float yOffset = windowSize.y / 5.0f;

	auto innerPoint1 = sf::Vector2f(xOffset * 2, yOffset * 2);
	auto innerPoint2 = sf::Vector2f(xOffset * 3, innerPoint1.y);
	auto innerPoint3 = sf::Vector2f(innerPoint2.x, yOffset * 3);
	auto innerPoint4 = sf::Vector2f(innerPoint1.x, innerPoint3.y);

	auto outerPoint1 = sf::Vector2f(xOffset, yOffset);
	auto outerPoint2 = sf::Vector2f(xOffset * 4, outerPoint1.y);
	auto outerPoint3 = sf::Vector2f(outerPoint2.x, yOffset * 4 + 10);
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
	m_vehicleCenter = sf::Vector2f(innerPoint2.x + xOffset * 0.5f, innerPoint2.y + yOffset * 0.5f);
	m_vehicleAngle = 90.0;
}

MapBuilder::MapBuilder() :
	AbstractBuilder(),
	m_edgesPivot(0),
	m_vehiclePositioned(false),
	m_vehicleAngle(0.0)
{
	m_operationsMap[ERROR_VEHICLE_IS_NOT_POSITIONED] = "Error: Vehicle is not positioned!";
	m_operationsMap[ERROR_VEHICLE_OUTSIDE_MAP_AREA] = "Error: Vehicle is outside map area!";
	m_operationsMap[ERROR_VEHICLE_OUTSIDE_ROAD_AREA] = "Error: Vehicle is outside road area!";
	m_operationsMap[ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE] = "Error: Vehicle's angle is too little!";
	m_operationsMap[ERROR_VEHICLE_ANGLE_IS_TOO_LARGE] = "Error: Vehicle's angle is too large!";
	m_operationsMap[ERROR_EDGES_ARE_NOT_SPECIFIED] = "Error: Edges are not specified!";
	m_operationsMap[ERROR_INCORRECT_EDGE_SEQUENCE_COUNT] = "Error: There should be only two edge sequences!";
	m_operationsMap[ERROR_EDGE_SEQUENCE_INTERSECTION] = "Error: Found intersection between edge sequences!";
	m_operationsMap[ERROR_TOO_LITTLE_INNER_EDGES] = "Error: Too little inner edges specified!";
	m_operationsMap[ERROR_TOO_MANY_INNER_EDGES] = "Error: Too many inner edges specified!";
	m_operationsMap[ERROR_DIFFERENT_NUMBER_OF_INNER_AND_OUTER_EDGES] = "Error: Number of inner edges does not match number of outer edges!";
	m_operationsMap[ERROR_TOO_LITTLE_OUTER_EDGES] = "Error: Too little outer edges specified!";
	m_operationsMap[ERROR_TOO_MANY_OUTER_EDGES] = "Error: Too many outer edges specified!";
	m_operationsMap[ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS] = "Error: Cannot generate all checkpoints! Check if edges are reasonable positioned.";
	Clear();
}

MapBuilder::~MapBuilder()
{
}

void MapBuilder::AddVehicle(double angle, sf::Vector2f center)
{
	m_vehiclePositioned = true;
	m_vehicleCenter = center;
	m_vehicleAngle = angle;
}

MapPrototype* MapBuilder::Get()
{
	if (!Validate())
		return nullptr;

	auto innerEdges = EdgeVector(m_edges.begin(), m_edges.begin() + m_edgesPivot);
	auto outerEdges = EdgeVector(m_edges.begin() + m_edgesPivot, m_edges.end());
	auto innerEdgesChain = EdgesChainGenerator::Generate(innerEdges, false);
	auto outerEdgesChain = EdgesChainGenerator::Generate(outerEdges, true);
	auto checkpoints = RectangleCheckpointsGenerator::Generate(innerEdges, outerEdges);
	return new MapPrototype(innerEdgesChain, outerEdgesChain, checkpoints);
}

bool MapBuilder::Initialize()
{
	// Initialize max allowed map area
	{
		auto size = CoreWindow::GetWindowSize() * 3.0f;
		auto position = sf::Vector2f(size.x / 20.0f, size.y / 20.0f);
		m_maxAllowedMapArea = std::make_pair(position, size);
	}

	// Initialize max allowed view area
	{
		auto area = m_maxAllowedMapArea;
		auto size = area.second * 1.05f;
		auto position = sf::Vector2f(area.first.x - (size.x - area.second.x) / 2, area.first.y - (size.y - area.second.y) / 2);
		m_maxAllowedViewArea = std::make_pair(position, size);
	}

	MapBuilder builder;

	// Call internal implementation
	builder.CreateDummy();

	if (!builder.Validate())
	{
		CoreLogger::PrintError("Cannot create MapPrototype dummy!");
		return false;
	}

	CoreLogger::PrintSuccess("MapBuilder initialized correctly");
	return true;
}
