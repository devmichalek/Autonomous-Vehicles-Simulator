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

				if (MathContext::Intersect(checkpoints[i], edgesChain[j]))
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
		const auto innerDistance = MathContext::Distance(beginEdge[0], endEdge[0]);
		const auto outerDistance = MathContext::Distance(beginEdge[1], endEdge[1]);
		const size_t innerCount = size_t(innerDistance / minWidth);
		const size_t outerCount = size_t(outerDistance / minWidth);
		const size_t count = (innerCount > outerCount) ? innerCount : outerCount;
		const float innerOffset = float(innerDistance) / float(count);
		const float outerOffset = float(outerDistance) / float(count);

		double innerAngle = MathContext::DifferenceVectorAngle(beginEdge[0], endEdge[0]);
		double outerAngle = MathContext::DifferenceVectorAngle(beginEdge[1], endEdge[1]);

		Edge previousEdge = beginEdge;
		for (size_t i = 1; i < count; ++i)
		{
			auto p1 = previousEdge[0];
			auto p2 = previousEdge[1];
			auto p3 = MathContext::GetEndPoint(beginEdge[1], outerAngle, -outerOffset * i);
			auto p4 = MathContext::GetEndPoint(beginEdge[0], innerAngle, -innerOffset * i);
			
			result.push_back({ p1, p2, p3, p4 });
			
			previousEdge[0] = p4;
			previousEdge[1] = p3;
		}

		result.push_back({ previousEdge[0], previousEdge[1], endEdge[1], endEdge[0] });
	};

	auto CheckCollision = [](const RectangleVector& checkpoints, const size_t index, const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain) {
		const size_t lastCheckpointIndex = checkpoints.size() - 1;
		const size_t numberOfEdgesPerChain = innerEdgesChain.size();
		for (size_t i = 1 ; i < lastCheckpointIndex; ++i)
		{
			const Edge edge = { checkpoints[i][0], checkpoints[i][1] };
			for (size_t j = 0; j < index; ++j)
			{
				if (MathContext::Intersect(edge, innerEdgesChain[j]))
					return false;

				if (MathContext::Intersect(edge, innerEdgesChain[j]))
					return false;
			}

			for (size_t j = index + 1; j < numberOfEdgesPerChain; ++j)
			{
				if (MathContext::Intersect(edge, innerEdgesChain[j]))
					return false;

				if (MathContext::Intersect(edge, innerEdgesChain[j]))
					return false;
			}
		}

		return true;
	};

	// Generate rectangle checkpoints
	const auto numberOfLineCheckpoints = lineCheckpoints.size();
	const auto lastLineCheckpointIndex = numberOfLineCheckpoints - 1;
	std::vector<RectangleVector> subresults(numberOfLineCheckpoints);
	for (size_t i = 0; i < lastLineCheckpointIndex; ++i)
		AddRectangleCheckpoints(subresults[i], lineCheckpoints[i], lineCheckpoints[i + 1]);
	AddRectangleCheckpoints(subresults[lastLineCheckpointIndex], lineCheckpoints[lastLineCheckpointIndex], lineCheckpoints[0]);

	// Check chepoints against edges and create result at the same time
	RectangleVector result;
	for (size_t i = 0; i < subresults.size(); ++i)
	{
		if (!CheckCollision(subresults[i], i, innerEdgesChain, outerEdgesChain))
			return {};
		result.insert(result.end(), subresults[i].begin(), subresults[i].end());
	}
		
	return result;
}

bool MapBuilder::ValidateMapAreaVehiclePosition()
{
	auto allowedMapArea = GetMaxAllowedMapArea();
	if (!MathContext::IsPointInsideRectangle(allowedMapArea.second, allowedMapArea.first, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_MAP_AREA;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateRoadAreaVehiclePosition()
{
	std::vector<sf::Vector2f> innerPoints(m_innerEdgesChain.size());
	for (size_t i = 0; i < m_innerEdgesChain.size(); ++i)
		innerPoints[i] = m_innerEdgesChain[i][0];

	// If vehicle center is inside inner polygon return error
	if (MathContext::IsPointInsidePolygon(innerPoints, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_ROAD_AREA;
		return false;
	}

	std::vector<sf::Vector2f> outerPoints(m_outerEdgesChain.size());
	for (size_t i = 0; i < outerPoints.size(); ++i)
		outerPoints[i] = m_outerEdgesChain[i][0];

	// If vehicle is not inside outer polygon return error
	if (!MathContext::IsPointInsidePolygon(outerPoints, m_vehicleCenter))
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

bool MapBuilder::ValidateNumberOfEdgesPerChain(size_t count)
{
	if (count < GetMinNumberOfEdgesPerChain())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_EDGES_PER_CHAIN;
		return false;
	}

	if (count > GetMaxNumberOfEdgesPerChain())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_EDGES_PER_CHAIN;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateEdgesChainsIntersection()
{
	for (const auto & innerEdge : m_innerEdgesChain)
	{
		for (const auto& outerEdge : m_outerEdgesChain)
		{
			if (MathContext::Intersect(innerEdge, outerEdge))
			{
				m_lastOperationStatus = ERROR_TWO_EDGES_INTERSECTION;
				return false;
			}
		}
	}

	return true;
}

bool MapBuilder::ValidatesEdgesChains()
{
	if (!MathContext::IsEdgesChain(m_innerEdgesChain))
	{
		m_lastOperationStatus = ERROR_INNER_EDGES_DO_NOT_CREATE_CHAIN;
		return false;
	}

	if (!MathContext::IsEdgesChain(m_outerEdgesChain))
	{
		m_lastOperationStatus = ERROR_OUTER_EDGES_DO_NOT_CREATE_CHAIN;
		return false;
	}

	return true;
}

bool MapBuilder::ValidateCheckpoints()
{
	if (RectangleCheckpointsGenerator::Generate(m_innerEdgesChain, m_outerEdgesChain).empty())
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

	if (!ValidateNumberOfEdgesPerChain(m_innerEdgesChain.size()))
		return false;

	if (!ValidateNumberOfEdgesPerChain(m_outerEdgesChain.size()))
		return false;

	if (!ValidateEdgesChainsIntersection())
		return false;

	if (!ValidatesEdgesChains())
		return false;

	if (!ValidateCheckpoints())
		return false;

	if (!ValidateRoadAreaVehiclePosition())
		return false;

	return true;
}

void MapBuilder::ClearInternal()
{
	m_innerEdgesChain.clear();
	m_outerEdgesChain.clear();
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

	// Read number of edges per chain
	size_t numberOfEdgesPerChain = 0;
	input.read((char*)&numberOfEdgesPerChain, sizeof(numberOfEdgesPerChain));

	if (!ValidateNumberOfEdgesPerChain(numberOfEdgesPerChain))
		return false;

	// Read inner edges points
	m_innerEdgesChain.resize(numberOfEdgesPerChain);
	for (auto& i : m_innerEdgesChain)
	{
		input.read((char*)&i[0].x, sizeof(i[0].x));
		input.read((char*)&i[0].y, sizeof(i[0].y));
	}

	// Read outer edges points
	m_outerEdgesChain.resize(numberOfEdgesPerChain);
	for (auto& i : m_outerEdgesChain)
	{
		input.read((char*)&i[0].x, sizeof(i[0].x));
		input.read((char*)&i[0].y, sizeof(i[0].y));
	}

	// Pin edges
	for (size_t i = 0; i < numberOfEdgesPerChain - 1; i++)
	{
		m_innerEdgesChain[i][1] = m_innerEdgesChain[i + 1][0];
		m_outerEdgesChain[i][1] = m_outerEdgesChain[i + 1][0];
	}
	m_innerEdgesChain[numberOfEdgesPerChain - 1][1] = m_innerEdgesChain[0][0];
	m_outerEdgesChain[numberOfEdgesPerChain - 1][1] = m_outerEdgesChain[0][0];

	// Validate edges chains intersection
	if (!ValidateEdgesChainsIntersection())
		return false;

	if (!ValidatesEdgesChains())
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
	
	// Save number of edges per chain
	size_t numberOfEdgesPerChain = m_innerEdgesChain.size();
	output.write((const char*)&numberOfEdgesPerChain, sizeof(numberOfEdgesPerChain));

	// Save inner edges beggining position
	for (auto& i : m_innerEdgesChain)
	{
		output.write((const char*)&i[0].x, sizeof(i[0].x));
		output.write((const char*)&i[0].y, sizeof(i[0].y));
	}

	// Save outer edges beggining position
	for (auto& i : m_outerEdgesChain)
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

	m_innerEdgesChain.push_back({ innerPoint1, innerPoint2 });
	m_innerEdgesChain.push_back({ innerPoint2, innerPoint3 });
	m_innerEdgesChain.push_back({ innerPoint3, innerPoint4 });
	m_innerEdgesChain.push_back({ innerPoint4, innerPoint1 });

	m_outerEdgesChain.push_back({ outerPoint1, outerPoint2 });
	m_outerEdgesChain.push_back({ outerPoint2, outerPoint3 });
	m_outerEdgesChain.push_back({ outerPoint3, outerPoint4 });
	m_outerEdgesChain.push_back({ outerPoint4, outerPoint1 });

	m_vehiclePositioned = true;
	m_vehicleCenter = sf::Vector2f(innerPoint2.x + xOffset * 0.5f, innerPoint2.y + yOffset * 0.5f);
	m_vehicleAngle = 90.0;
}

MapBuilder::MapBuilder() :
	AbstractBuilder(),
	m_vehiclePositioned(false),
	m_vehicleAngle(0.0)
{
	m_operationsMap.insert(std::pair(ERROR_VEHICLE_IS_NOT_POSITIONED, "Error: Vehicle is not positioned!"));
	m_operationsMap.insert(std::pair(ERROR_VEHICLE_OUTSIDE_MAP_AREA, "Error: Vehicle is outside map area!"));
	m_operationsMap.insert(std::pair(ERROR_VEHICLE_OUTSIDE_ROAD_AREA, "Error: Vehicle is outside road area!"));
	m_operationsMap.insert(std::pair(ERROR_VEHICLE_ANGLE_IS_TOO_LITTLE, "Error: Vehicle's angle is too little!"));
	m_operationsMap.insert(std::pair(ERROR_VEHICLE_ANGLE_IS_TOO_LARGE, "Error: Vehicle's angle is too large!"));
	m_operationsMap.insert(std::pair(ERROR_EDGES_ARE_NOT_SPECIFIED, "Error: Edges are not specified!"));
	m_operationsMap.insert(std::pair(ERROR_TWO_EDGES_INTERSECTION, "Error: Found intersection between two edges!"));
	m_operationsMap.insert(std::pair(ERROR_INNER_EDGES_DO_NOT_CREATE_CHAIN, "Error: Inner edges do not create edges chain!"));
	m_operationsMap.insert(std::pair(ERROR_OUTER_EDGES_DO_NOT_CREATE_CHAIN, "Error: Outer edges do not create edges chain!"));
	m_operationsMap.insert(std::pair(ERROR_TOO_LITTLE_EDGES_PER_CHAIN, "Error: There are too little edges per chain!"));
	m_operationsMap.insert(std::pair(ERROR_TOO_MANY_EDGES_PER_CHAIN, "Error: There are too many edges per chain!"));
	m_operationsMap.insert(std::pair(ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS, "Error: Cannot generate all checkpoints! Check if edges are reasonable positioned."));
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

	auto innerEdgesChain = EdgesChainGenerator::Generate(m_innerEdgesChain, false);
	auto outerEdgesChain = EdgesChainGenerator::Generate(m_outerEdgesChain, true);
	auto checkpoints = RectangleCheckpointsGenerator::Generate(m_innerEdgesChain, m_outerEdgesChain);
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
