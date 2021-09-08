#include "DrawableMapBuilder.hpp"
#include "DrawableMap.hpp"
#include "CoreLogger.hpp"

bool DrawableMapBuilder::ValidateAllowedAreaVehiclePosition()
{
	auto allowedMapArea = GetMaxAllowedMapArea();
	if (!DrawableMath::IsPointInsideRectangle(allowedMapArea.second, allowedMapArea.first, m_vehicleCenter))
	{
		m_lastOperationStatus = ERROR_VEHICLE_OUTSIDE_ALLOWED_MAP_AREA;
		return false;
	}

	return true;
}

bool DrawableMapBuilder::ValidateRoadAreaVehiclePosition()
{

	return true;
}

bool DrawableMapBuilder::ValidateVehicleAngle()
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

bool DrawableMapBuilder::ValidateTotalNumberOfEdges(size_t count)
{
	if (count == 0)
	{
		m_lastOperationStatus = ERROR_EDGES_ARE_NOT_SPECIFIED;
		return false;
	}

	return true;
}

bool DrawableMapBuilder::ValidateEdgesPivot(size_t pivot, size_t count)
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

bool DrawableMapBuilder::ValidateNumberOfEdgeSequences()
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

bool DrawableMapBuilder::ValidateEdgeSequenceIntersection()
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

bool DrawableMapBuilder::ValidateTriangleCheckpoints()
{
	auto checkpoints = DrawableMap::GenerateTriangleCheckpoints(m_edges, m_edgesPivot);
	if (checkpoints.empty())
	{
		m_lastOperationStatus = ERROR_CANNOT_GENERATE_ALL_CHECKPOINTS;
		return false;
	}

	return true;
}

bool DrawableMapBuilder::ValidateInternal()
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

void DrawableMapBuilder::ClearInternal()
{
	m_edgesPivot = 0;
	m_edges.clear();
	m_vehiclePositioned = false;
}

bool DrawableMapBuilder::LoadInternal(std::ifstream& input)
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

bool DrawableMapBuilder::SaveInternal(std::ofstream& output)
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
	m_vehicleCenter = sf::Vector2f(innerPoint2.x + xOffset * 0.5f, innerPoint2.y + yOffset * 0.5f);
	m_vehicleAngle = 90.0;
}

DrawableMapBuilder::DrawableMapBuilder() :
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

void DrawableMapBuilder::UpdateVehicle(DrawableVehicle* drawableVehicle)
{
	if (drawableVehicle)
	{
		drawableVehicle->SetCenter(m_vehicleCenter);
		drawableVehicle->SetAngle(m_vehicleAngle);
		drawableVehicle->Update();
	}
}

DrawableMap* DrawableMapBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return new DrawableMap(m_edges, m_edgesPivot);
}

DrawableMap* DrawableMapBuilder::Copy(const DrawableMap* drawableMap)
{
	if (!drawableMap)
		return nullptr;

	return new DrawableMap(*drawableMap);
}

size_t DrawableMapBuilder::GetMinNumberOfInnerEdges()
{
	return 4;
}

size_t DrawableMapBuilder::GetMaxNumberOfInnerEdges()
{
	return 512;
}

size_t DrawableMapBuilder::GetMinNumberOfOuterEdges()
{
	return GetMinNumberOfInnerEdges();
}

size_t DrawableMapBuilder::GetMaxNumberOfOuterEdges()
{
	return GetMaxNumberOfInnerEdges();
}

std::pair<sf::Vector2f, sf::Vector2f> DrawableMapBuilder::GetMaxAllowedMapArea() const
{
	auto size = CoreWindow::GetSize() * 3.0f;
	auto position = sf::Vector2f(size.x / 20.0f, size.y / 20.0f);
	return std::make_pair(position, size);
}

std::pair<sf::Vector2f, sf::Vector2f> DrawableMapBuilder::GetMaxAllowedViewArea() const
{
	auto area = GetMaxAllowedMapArea();
	auto size = area.second * 1.05f;
	auto position = sf::Vector2f(area.first.x - (size.x - area.second.x) / 2, area.first.y - (size.y - area.second.y) / 2);
	return std::make_pair(position, size);
}

double DrawableMapBuilder::GetMinVehicleAngle()
{
	return 0.0;
}

double DrawableMapBuilder::GetMaxVehicleAngle()
{
	return 360.0;
}

bool DrawableMapBuilder::Initialize()
{
	DrawableMapBuilder builder;

	// Call internal implementation
	builder.CreateDummy();

	if (!builder.Validate())
	{
		CoreLogger::PrintError("Cannot create Drawable Map dummy!");
		return false;
	}

	return true;
}
