#pragma once
#include "DrawableMap.hpp"

std::vector<std::pair<size_t, size_t>> DrawableMap::GetAvailableEndPoints(size_t index)
{
	auto edgesCount = m_edges.size();
	using EndPoint = std::tuple<size_t, size_t, double>; // Consists of index of an edge and distance to its first point
	std::vector<EndPoint> endPoints;
	for (size_t j = m_pivot; j < edgesCount; ++j)
	{
		bool intersection = false;
		Edge edge = { m_edges[index][0], m_edges[j][0] };
		for (size_t k = 0; k < edgesCount; ++k)
		{
			// Skip
			if (index == k || j == k)
				continue;

			// Check if there is no intersection with any edge
			if (DrawableMath::IntersectNonCollinear(edge, m_edges[k]))
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

DrawableMap::DrawableMap(const EdgeVector& edges, const size_t pivot) :
	m_pivot(pivot)
{
	m_edgeLine[0].color = sf::Color::White;
	m_edgeLine[1].color = m_edgeLine[0].color;
	m_edges = edges;

	m_fitnessVector.clear();
	m_previousFitnessVector.clear();
	m_highestFitness = 0;
	m_highestFitnessOverall = 0;
	m_timers.clear();
	m_minFitnessImprovement = 0.0;
	m_triangleCheckpoints.clear();

	// Gather available end points for each point from inner edge sequence
	std::vector<std::vector<std::pair<size_t, size_t>>> availableEndPoints(pivot);
	for (size_t i = 0; i < pivot; ++i)
		availableEndPoints[i] = GetAvailableEndPoints(i);

	// Start adding checkpoints, add checkpoint only if there is no intersection with other checkpoint
	using EdgePrecedence = std::pair<Edge, size_t>; // Edge + Precedence
	using LocalCheckpoints = std::vector<EdgePrecedence>;
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
			auto endPointPrecedence = availableEndPoints[i].back().second;
			Edge edge = { edges[i][0], edges[endPointIndex][0] };

			bool intersection = false;
			for (size_t k = 0; k < checkpoints.size(); ++k)
			{
				if (k == i)
					continue;

				for (size_t c = 0; c < checkpoints[k].size(); ++c)
				{
					if (DrawableMath::IntersectNonCollinear(edge, checkpoints[k][c].first))
					{
						intersection = true;
						break;
					}
				}

				if (intersection)
					break;
			}

			// Remove last end point
			availableEndPoints[i].pop_back();

			// Add local checkpoint if there was no intersection
			if (!intersection)
				checkpoints[i].push_back(std::make_pair(edge, endPointPrecedence));
		}
	}

	// Sort checkpoints
	auto Compare = [&](const EdgePrecedence& a, const EdgePrecedence& b)
	{
		return a.second < b.second;
	};

	for (size_t i = 0; i < pivot; ++i)
		std::sort(checkpoints[i].begin(), checkpoints[i].end(), Compare);

	// Add checkpoints as triangles
	auto checkpointCount = checkpoints.size();
	for (size_t i = 1; i < checkpointCount; ++i)
	{
		size_t lastIndex = checkpoints[i].size() - 1;
		for (size_t j = 0; j < lastIndex; ++j)
		{
			Triangle triangle;
			triangle[0] = checkpoints[i][j].first[0];
			triangle[1] = checkpoints[i][j].first[1];
			triangle[2] = checkpoints[i][j + 1].first[1];
			m_triangleCheckpoints.push_back(triangle);
			m_lineCheckpoints.push_back(checkpoints[i][j].first);
		}

		Triangle triangle;
		triangle[0] = checkpoints[i][lastIndex].first[0];
		triangle[1] = checkpoints[i][lastIndex].first[1];
		if (i + 1 >= checkpointCount)
		{
			triangle[2] = m_edges[0][0];
			m_triangleCheckpoints.push_back(triangle);

			Triangle triangle;
			triangle[0] = m_edges.back()[0];
			triangle[1] = m_edges.back()[1];
			triangle[2] = m_edges.front()[0];
			m_triangleCheckpoints.push_back(triangle);
		}
		else
		{
			triangle[2] = m_edges[i + 1][0];
			m_triangleCheckpoints.push_back(triangle);
		}
		
		m_lineCheckpoints.push_back(checkpoints[i][lastIndex].first);
	}

	m_lineCheckpointShape[0].color = sf::Color::Red;
	m_lineCheckpointShape[1].color = m_lineCheckpointShape[0].color;

	// Set shape point count
	m_triangleCheckpointShape.setPointCount(m_triangleCheckpoints.back().size());

	// Add finish line position
	m_edges.push_back({ m_edges.front()[0], m_edges[pivot][0] });

	// Set blocking edge position
	m_edges.push_back({ m_edges.front()[1], m_edges[pivot][1] });

	// Erase continuity
	//m_edges.erase(m_edges.begin() + pivot);
	//m_edges.erase(m_edges.begin());
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
	for (size_t i = 0; i < m_triangleCheckpoints.size(); ++i)
	{
		for (size_t j = 0; j < m_triangleCheckpointShape.getPointCount(); ++j)
			m_triangleCheckpointShape.setPoint(j, m_triangleCheckpoints[i][j]);

		sf::Uint8 red = 255 * (i % 3);
		sf::Uint8 green = 255 * ((i + 1) % 3);
		sf::Uint8 blue = 255 * ((i + 2) % 3);
		m_triangleCheckpointShape.setFillColor(sf::Color(red, green, blue, 48));
		CoreWindow::GetRenderWindow().draw(m_triangleCheckpointShape);
	}

	/*for (auto& position : m_lineCheckpoints)
	{
		m_lineCheckpointShape[0].position = position[0];
		m_lineCheckpointShape[1].position = position[1];
		CoreWindow::GetRenderWindow().draw(m_lineCheckpointShape.data(), m_lineCheckpointShape.size(), sf::Lines);
	}*/
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
	for (size_t i = 0; i < m_triangleCheckpoints.size(); ++i)
	{
		if (DrawableMath::Intersect(m_triangleCheckpoints[i], drawableVehicle->GetVertices()))
			fitness = Fitness(i + 1);
	}

	return fitness;
}

Fitness DrawableMap::GetMaxFitness()
{
	return Fitness(m_triangleCheckpoints.size());
}
