#include "MapBuilder.hpp"
#include "CoreLogger.hpp"
#include "CoreWindow.hpp"

std::pair<sf::Vector2f, sf::Vector2f> MapBuilder::m_maxAllowedMapArea;
std::pair<sf::Vector2f, sf::Vector2f> MapBuilder::m_maxAllowedViewArea;

bool MapBuilder::EdgesChainGenerator::IsClockwiseOrder(const EdgeVector& edges, const size_t count)
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

EdgeVector MapBuilder::EdgesChainGenerator::Generate(const EdgeVector& edges, const size_t pivot, bool& clockwise)
{
	// Determine order
	clockwise = IsClockwiseOrder(edges, pivot);

	const Edge firstInnerEdge = edges[0];
	const Edge firstOuterEdge = edges[pivot];

	EdgeVector edgesChain(edges.size());

	// Add blocking edge
	edgesChain.front() = { firstOuterEdge[1], firstInnerEdge[1] };

	// Create chain order
	for (size_t i = 1; i < pivot; ++i)
		edgesChain[i] = edges[i];

	// Add finish line
	edgesChain[pivot] = { firstInnerEdge[0], firstOuterEdge[0] };

	// Create chain order
	for (size_t i = pivot + 1, j = edges.size() - 1; i < edges.size(); ++i, --j)
	{
		edgesChain[i][1] = edges[j][0];
		edgesChain[i][0] = edges[j][1];
	}

	return edgesChain;
}

std::vector<std::pair<size_t, size_t>> MapBuilder::TriangleCheckpointsGenerator::GetEndPoints(const EdgeVector& edges, const size_t pivot, const size_t index)
{
	auto edgesCount = edges.size();
	using EndPoint = std::tuple<size_t, size_t, double>; // Consists of index of an edge and distance to its first point
	std::vector<EndPoint> endPoints;
	for (size_t j = pivot + 1; j < edgesCount; ++j)
	{
		bool intersection = false;
		Edge edge = { edges[index][0], edges[j][0] };
		for (size_t k = 0; k < edgesCount; ++k)
		{
			// Skip
			if (index == k || j == k)
				continue;

			// Check if there is no intersection with any edge
			if (DrawableMath::IntersectNonCollinear(edge, edges[k]))
			{
				intersection = true;
				break;
			}
		}

		// There was no intersection
		if (!intersection)
		{
			// Get proper precedence
			size_t precedence = endPoints.empty() ? 0 : (std::get<1>(endPoints.back()) + 1);

			// Calculate distance
			auto distance = DrawableMath::Distance(edge);
			endPoints.push_back(std::make_tuple(j, precedence, distance));
		}
	}

	// Sort from the smallest distance to the greatest
	auto Compare = [&](const EndPoint& a, const EndPoint& b)
	{
		return std::get<2>(a) > std::get<2>(b);
	};
	std::sort(endPoints.begin(), endPoints.end(), Compare);

	std::vector<std::pair<size_t, size_t>> result(endPoints.size());
	for (size_t i = 0; i < result.size(); ++i)
	{
		result[i].first = std::get<0>(endPoints[i]);
		result[i].second = std::get<1>(endPoints[i]);
	}
	return result;
}

std::vector<EdgeVector> MapBuilder::TriangleCheckpointsGenerator::GetLineCheckpoints(const EdgeVector& edges, const size_t pivot, EndPointsVector& endPointsVector)
{
	using EdgePrecedence = std::pair<Edge, size_t>;
	using EdgePrecedences = std::vector<EdgePrecedence>;
	using EdgePrecedencesVector = std::vector<EdgePrecedences>;
	EdgePrecedencesVector edgePrecedencesVector(pivot, EdgePrecedences());

	bool process = true;
	while (process)
	{
		process = false;
		for (size_t i = 0; i < pivot; ++i)
		{
			if (endPointsVector[i].empty())
				continue;
			process = true;

			auto endPointIndex = endPointsVector[i].back().first;
			auto endPointPrecedence = endPointsVector[i].back().second;
			Edge edge = { edges[i][0], edges[endPointIndex][0] };

			bool intersection = false;
			for (size_t k = 0; k < edgePrecedencesVector.size(); ++k)
			{
				if (k == i)
					continue;

				for (size_t c = 0; c < edgePrecedencesVector[k].size(); ++c)
				{
					if (DrawableMath::IntersectNonCollinear(edge, edgePrecedencesVector[k][c].first))
					{
						intersection = true;
						break;
					}
				}

				if (intersection)
					break;
			}

			// Remove last end point
			endPointsVector[i].pop_back();

			// Add EdgePrecedence if there was no intersection
			if (!intersection)
				edgePrecedencesVector[i].push_back(std::make_pair(edge, endPointPrecedence));
		}
	}

	auto Compare = [&](const EdgePrecedence& a, const EdgePrecedence& b)
	{
		return a.second < b.second;
	};

	for (auto& edgePrecedences : edgePrecedencesVector)
		std::sort(edgePrecedences.begin(), edgePrecedences.end(), Compare);

	// Add more edge precedences to have optimal number of checkpoints per edge
	double maxDistanceBetweenEndPoints = CoreWindow::GetWindowSize().x / 16;
	auto checkpointCount = edgePrecedencesVector.size();
	for (size_t i = 1; i < checkpointCount; ++i)
	{
		auto& edgePrecedences = edgePrecedencesVector[i];
		if (edgePrecedences.empty())
			return {};

		size_t lastIndex = edgePrecedences.size() - 1;
		for (size_t j = 0; j < lastIndex; ++j)
		{
			auto p2 = edgePrecedences[j].first[1];
			auto p3 = edgePrecedences[j + 1].first[1];
			double distance = DrawableMath::Distance({ p2, p3 });
			double times = distance / maxDistanceBetweenEndPoints;
			size_t repeat = static_cast<size_t>(times);
			if (repeat-- > 1)
			{
				auto p1 = edgePrecedences[j].first[0];
				auto angle = DrawableMath::DifferenceVectorAngle(p2, p3);
				const double offset = -distance / double(repeat + 1);
				EdgePrecedences newEdgePrecedences(repeat);
				for (size_t k = 0; k < repeat; ++k)
				{
					auto p4 = DrawableMath::GetEndPoint(p2, angle, (offset * (k + 1)));
					newEdgePrecedences[k] = { { p1, p4 }, edgePrecedences[j].second + 1 + k };
				}

				edgePrecedences.insert(edgePrecedences.begin() + j + 1, newEdgePrecedences.begin(), newEdgePrecedences.end());
				j += repeat;
				lastIndex += repeat;
				for (size_t k = j + 1; k <= lastIndex; ++k)
					edgePrecedences[k].second += repeat;
			}
		}

		size_t nextIndex = (i + 1 >= checkpointCount) ? 0 : (i + 1);
		auto p2 = edgePrecedences[lastIndex].first[0];
		auto p3 = edgePrecedencesVector[nextIndex][0].first[0];
		double distance = DrawableMath::Distance({ p2, p3 });
		double times = distance / maxDistanceBetweenEndPoints;
		size_t repeat = static_cast<size_t>(times);
		if (repeat-- > 1)
		{
			auto p1 = edgePrecedences[lastIndex].first[1];
			auto angle = DrawableMath::DifferenceVectorAngle(p2, p3);
			const double offset = -distance / double(repeat + 1);
			for (size_t k = 0; k < repeat; ++k)
			{
				auto p4 = DrawableMath::GetEndPoint(p2, angle, (offset * (k + 1)));
				EdgePrecedences newEdgePrecedences = { { { p4, p1 }, 0 } };
				edgePrecedencesVector.insert(edgePrecedencesVector.begin() + ++i, newEdgePrecedences);
				++checkpointCount;
			}
		}
	}

	// Convert edge precedences vector to edge vector
	std::vector<EdgeVector> result(checkpointCount);
	for (size_t i = 0; i < checkpointCount; ++i)
	{
		auto& edgePrecedences = edgePrecedencesVector[i];
		auto count = edgePrecedences.size();
		result[i].resize(count);
		for (size_t j = 0; j < count; ++j)
			result[i][j] = edgePrecedences[j].first;
	}

	return result;
}

TriangleVector MapBuilder::TriangleCheckpointsGenerator::GetTriangleCheckpoints(const std::vector<EdgeVector>& edgeVectors)
{
	TriangleVector result;
	auto checkpointCount = edgeVectors.size();
	for (size_t i = 1; i < checkpointCount; ++i)
	{
		auto& edgeVector = edgeVectors[i];
		size_t lastIndex = edgeVector.size() - 1;
		for (size_t j = 0; j < lastIndex; ++j)
			result.push_back({ edgeVector[j][0], edgeVector[j][1], edgeVector[j + 1][1] });

		size_t nextIndex = (i + 1 >= checkpointCount) ? 0 : (i + 1);
		result.push_back({ edgeVector[lastIndex][0], edgeVector[lastIndex][1], edgeVectors[nextIndex][0][0] });
	}

	result.shrink_to_fit();

	return result;
}

TriangleVector MapBuilder::TriangleCheckpointsGenerator::Generate(const EdgeVector& edges, const size_t pivot)
{
	// Gather available end points for each point from inner edge sequence
	EndPointsVector endPointsVector(pivot);
	for (size_t i = 0; i < pivot; ++i)
	{
		endPointsVector[i] = GetEndPoints(edges, pivot, i);
		if (endPointsVector[i].empty())
			return {};
	}

	// Generate edge precendences vector
	auto edgeVectors = GetLineCheckpoints(edges, pivot, endPointsVector);
	if (edgeVectors.empty())
		return {};

	// Generate triangle checkpoints
	return GetTriangleCheckpoints(edgeVectors);
}

bool MapBuilder::ValidateAllowedAreaVehiclePosition()
{
	auto allowedMapArea = GetMaxAllowedMapArea();
	if (!DrawableMath::IsPointInsideRectangle(allowedMapArea.second, allowedMapArea.first, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_ALLOWED_MAP_AREA;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateRoadAreaVehiclePosition()
{

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

bool MapBuilder::ValidateTriangleCheckpoints()
{
	auto checkpoints = TriangleCheckpointsGenerator::Generate(m_edges, m_edgesPivot);
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

	if (!ValidateAllowedAreaVehiclePosition())
		return false;

	if (!ValidateRoadAreaVehiclePosition())
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

	if (!ValidateTriangleCheckpoints())
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

	if (!ValidateAllowedAreaVehiclePosition())
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

	// Validate triangle checkpoints
	if (!ValidateTriangleCheckpoints())
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
	m_operationsMap[ERROR_VEHICLE_OUTSIDE_ALLOWED_MAP_AREA] = "Error: Vehicle is outside allowed map area!";
	m_operationsMap[ERROR_VEHICLE_OUTSIDE_ALLOWED_ROAD_AREA] = "Error: Vehicle is outside allowed road area!";
	m_operationsMap[ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE] = "Error: Vehicle's angle is too little!";
	m_operationsMap[ERROR_VEHICLE_ANGLE_IS_TOO_LARGE] = "Error: Vehicle's angle is too large!";
	m_operationsMap[ERROR_EDGES_ARE_NOT_SPECIFIED] = "Error: Edges are not specified!";
	m_operationsMap[ERROR_INCORRECT_EDGE_SEQUENCE_COUNT] = "Error: There should be only two edge sequences!";
	m_operationsMap[ERROR_EDGE_SEQUENCE_INTERSECTION] = "Error: Found intersection between edge sequences!";
	m_operationsMap[ERROR_TOO_LITTLE_INNER_EDGES] = "Error: Too little inner edges specified!";
	m_operationsMap[ERROR_TOO_MANY_INNER_EDGES] = "Error: Too many inner edges specified!";
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

	bool clockwise = false;
	auto edgesChain = EdgesChainGenerator::Generate(m_edges, m_edgesPivot, clockwise);
	auto checkpoints = TriangleCheckpointsGenerator::Generate(m_edges, m_edgesPivot);
	return new MapPrototype(edgesChain, checkpoints, clockwise);
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
		CoreLogger::PrintError("Cannot create Map dummy!");
		return false;
	}

	return true;
}
