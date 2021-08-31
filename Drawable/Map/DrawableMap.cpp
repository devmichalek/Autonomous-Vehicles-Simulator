#pragma once
#include "DrawableMap.hpp"

DrawableMap::DrawableMap(const EdgeVector& edges, const size_t pivot) :
	m_pivot(pivot)
{
	m_fitnessVector.clear();
	m_previousFitnessVector.clear();
	m_highestFitness = 0;
	m_highestFitnessOverall = 0;
	m_timers.clear();
	m_minFitnessImprovement = 0.0;
	m_checkpoints.clear();

	const Edge& finishLine = edges[edges.size() - 1];
	const Edge& blockingEdge = edges[edges.size() - 2];

	using EndPoint = std::pair<size_t, double>; // Consists of point and distance to this point
	std::vector<std::vector<EndPoint>> availableEndPoints;
	const size_t edgesCount = edges.size();

	// Gather available end points for each point from inner edge sequence
	for (size_t i = 0; i < pivot; ++i)
	{
		std::vector<EndPoint> endPoints;
		for (size_t j = pivot; j < edgesCount; ++j)
		{
			bool intersection = false;
			Edge edge = { edges[i][0], edges[j][0] };
			for (size_t k = 0; k < edgesCount; ++k)
			{
				if (k == i || k == j)
					continue;

				// Check if there is no intersection with any edge
				if (DrawableMath::IntersectLoose(edge, edges[k]))
				{
					intersection = true;
					break;
				}
			}

			// There was no intersection
			if (!intersection)
			{
				// Calculate distance
				auto distance = DrawableMath::Distance(edge);
				endPoints.push_back(std::make_pair(j, distance));
			}
		}

		// Sort from the smallest distance to the greatest
		auto Compare = [&](const EndPoint& a, const EndPoint& b) { return a.second > b.second; };
		std::sort(endPoints.begin(), endPoints.end(), Compare);

		// Insert end points
		availableEndPoints.push_back(endPoints);
	}

	// Start adding checkpoints, add checkpoint only if there is no intersection with other checkpoint
	using EdgeAngle = std::pair<Edge, double>; // Edge + its angle against x axis
	using LocalCheckpoints = std::vector<EdgeAngle>; // Vector of edge angles for particular checkpoint
	std::vector<LocalCheckpoints> checkpoints(pivot, LocalCheckpoints());
	bool process = true;
	while (process)
	{
		process = false;
		for (size_t i = 0; i < pivot; ++i)
		{
			if (availableEndPoints[i].empty())
				continue;
			process = true;

			auto endPointIndex = availableEndPoints[i].back().first;
			Edge edge = { edges[i][0], edges[endPointIndex][0] };

			bool intersection = false;
			for (size_t k = 0; !intersection && k < checkpoints.size(); ++k)
			{
				for (size_t c = 0; c < checkpoints[k].size(); ++c)
				{
					if (DrawableMath::IntersectLoose(std::get<0>(checkpoints[k][c]), edge))
					{
						intersection = true;
						break;
					}
				}
			}

			// Remove end point
			availableEndPoints[i].erase(availableEndPoints[i].end() - 1);

			if (!intersection)
			{
				// Calculate angle
				auto angle = DrawableMath::DifferenceVectorAngle(edge[0], edge[1]);
				angle = DrawableMath::CastAtan2ToFullAngle(angle);

				// Push back
				checkpoints[i].push_back(std::make_pair(edge, angle));
			}
		}
	}

	// Calculate base angles
	std::vector<double> angleBordersVector(pivot);
	for (size_t i = 1; i < pivot; ++i)
		angleBordersVector[i] = DrawableMath::CastAtan2ToFullAngle(DrawableMath::DifferenceVectorAngle(edges[i - 1][1], edges[i - 1][0]));
	angleBordersVector[0] = DrawableMath::CastAtan2ToFullAngle(DrawableMath::DifferenceVectorAngle(edges[pivot - 1][1], edges[pivot - 1][0]));

	// Sort checkpoints
	for (size_t i = 0; i < pivot; ++i)
	{
		double baseAngle = angleBordersVector[i];
		auto Difference = [&](EdgeAngle& edgeAngle) {
			double result = baseAngle - edgeAngle.second;
			if (result < 0)
				edgeAngle.second = (baseAngle + 360.0) - edgeAngle.second;
			else
				edgeAngle.second = result;
		};
		std::for_each(checkpoints[i].begin(), checkpoints[i].end(), Difference);

		auto Compare = [&](const EdgeAngle& a, const EdgeAngle& b) {
			return a.second < b.second;
		};
		std::sort(checkpoints[i].begin(), checkpoints[i].end(), Compare);
	}

	// Add checkpoints as triangles
	auto checkpointCount = checkpoints.size();
	for (size_t i = 0; i < checkpointCount; ++i)
	{
		size_t lastIndex = checkpoints[i].size() - 1;
		for (size_t j = 0; j < lastIndex; ++j)
		{
			Triangle triangle;
			triangle[0] = checkpoints[i][j].first[0];
			triangle[1] = checkpoints[i][j].first[1];
			triangle[2] = checkpoints[i][j + 1].first[1];
			m_checkpoints.push_back(triangle);
		}

		Triangle triangle;
		triangle[0] = checkpoints[i][lastIndex].first[0];
		triangle[1] = checkpoints[i][lastIndex].first[1];
		if (i + 1 >= checkpointCount)
			triangle[2] = checkpoints[0][0].first[0];
		else
			triangle[2] = checkpoints[i + 1][0].first[0];
		m_checkpoints.push_back(triangle);
	}

	if (!m_checkpoints.empty())
	{
		// Erase continuity
		m_checkpoints.erase(m_checkpoints.begin());
		m_checkpoints.erase(m_checkpoints.begin());

		// Set shape point count
		m_shape.setPointCount(m_checkpoints.back().size());
	}

	// Set edges
	m_edgeLine[0].color = sf::Color::White;
	m_edgeLine[1].color = m_edgeLine[0].color;
	m_edges = edges;

	// Add finish line position
	Edge finishLineEdge;
	finishLineEdge[0] = m_edges.front()[0];
	finishLineEdge[1] = m_edges[pivot][0];
	m_edges.push_back(finishLineEdge);

	// Set blocking edge position
	Edge blockEdge;
	blockEdge[0] = m_edges.front()[1];
	blockEdge[1] = m_edges[pivot][1];
	m_edges.insert(m_edges.begin(), blockEdge);

	// Erase continuity
	m_edges.erase(m_edges.begin() + pivot + 1);
	m_edges.erase(m_edges.begin() + 1);
}

DrawableMap::~DrawableMap()
{
}

void DrawableMap::Reset()
{
	m_highestFitness = 0;
	m_highestFitnessOverall = 0;
	m_minFitnessImprovement = 0.0;

	for (size_t i = 0; i < m_fitnessVector.size(); ++i)
	{
		m_fitnessVector[i] = 0;
		m_previousFitnessVector[i] = 0.0;
		m_timers[i].Reset();
	}
}

void DrawableMap::Init(size_t size, double minFitnessImprovement)
{
	Reset();
	m_fitnessVector.resize(size, 0);
	m_previousFitnessVector.resize(size, 0.0);
	m_timers.resize(size, StoppableTimer(0.0, std::numeric_limits<double>::max()));
	m_minFitnessImprovement = minFitnessImprovement;
}

void DrawableMap::Intersect(DrawableVehicleFactory& drawableVehicleFactory)
{
	for (auto& vehicle : drawableVehicleFactory)
	{
		if (!vehicle->IsActive())
			continue;

		for (auto& edge : m_edges)
		{
			if (DrawableMath::Intersect(edge, vehicle->GetVertices()))
			{
				vehicle->SetInactive();
			}
			else
				vehicle->Detect(edge);
		}
	}
}

void DrawableMap::Draw()
{
	for (const auto& i : m_edges)
	{
		m_edgeLine[0].position = i[0];
		m_edgeLine[1].position = i[1];
		CoreWindow::GetRenderWindow().draw(m_edgeLine.data(), m_edgeLine.size(), sf::Lines);
	}
}

void DrawableMap::DrawDebug()
{
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		for (size_t j = 0; j < m_shape.getPointCount(); ++j)
			m_shape.setPoint(j, m_checkpoints[i][j]);

		sf::Uint8 red = 255 * (i % 3);
		sf::Uint8 green = 255 * ((i + 1) % 3);
		sf::Uint8 blue = 255 * ((i + 2) % 3);
		m_shape.setFillColor(sf::Color(red, green, blue, 48));
		CoreWindow::GetRenderWindow().draw(m_shape);
	}
}

void DrawableMap::Iterate(DrawableVehicleFactory& drawableVehicleFactory)
{
	auto maxFitness = GetMaxFitness();
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		m_fitnessVector[i] = CalculateFitness(drawableVehicleFactory[i]);
		m_fitnessVector[i] += static_cast<Fitness>(double(maxFitness) / m_timers[i].Value());
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;
}

size_t DrawableMap::MarkLeader(DrawableVehicleFactory& drawableVehicleFactory)
{
	auto maxFitness = GetMaxFitness();
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		drawableVehicleFactory[i]->SetFollowerColor();
		if (!drawableVehicleFactory[i]->IsActive())
		{
			m_fitnessVector[i] = 0;
			continue;
		}

		m_fitnessVector[i] = CalculateFitness(drawableVehicleFactory[i]);
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;

	size_t index = std::distance(m_fitnessVector.begin(), iterator);
	drawableVehicleFactory[index]->SetLeaderColor();
	return index;
}

std::pair<size_t, double> DrawableMap::Punish(DrawableVehicleFactory& drawableVehicleFactory)
{
	size_t population = 0;
	double meanRequiredFitness = 0;
	auto maxFitness = GetMaxFitness();
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		if (!drawableVehicleFactory[i]->IsActive())
			continue;

		++population;
		m_fitnessVector[i] = CalculateFitness(drawableVehicleFactory[i]);
		auto requiredFitness = m_previousFitnessVector[i];
		requiredFitness += double(maxFitness) * m_minFitnessImprovement;
		if (Fitness(requiredFitness) > m_fitnessVector[i])
			drawableVehicleFactory[i]->SetInactive();

		if (requiredFitness > double(m_fitnessVector[i]))
			m_previousFitnessVector[i] = requiredFitness;
		else
			m_previousFitnessVector[i] = double(m_fitnessVector[i]);

		meanRequiredFitness += m_previousFitnessVector[i];
	}

	return std::pair(population, meanRequiredFitness / population);
}

void DrawableMap::UpdateTimers()
{
	for (auto& timer : m_timers)
		timer.Update();
}

const FitnessVector& DrawableMap::GetFitnessVector() const
{
	return m_fitnessVector;
}

const Fitness& DrawableMap::GetHighestFitness() const
{
	return m_highestFitness;
}

const Fitness& DrawableMap::GetHighestFitnessOverall() const
{
	return m_highestFitnessOverall;
}

Fitness DrawableMap::CalculateFitness(DrawableVehicle* drawableVehicle)
{
	Fitness fitness = 0;
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		if (DrawableMath::Intersect(m_checkpoints[i], drawableVehicle->GetVertices()))
			fitness = Fitness(i + 1);
	}

	return fitness;
}

Fitness DrawableMap::GetMaxFitness()
{
	return Fitness(m_checkpoints.size());
}
