#pragma once
#include "DrawableMap.hpp"

DrawableMap::DrawableMap(const EdgeVector& edges, const size_t pivot) :
	m_edgesPivot(pivot)
{
	m_edgeLine[0].color = sf::Color::White;
	m_edgeLine[1].color = m_edgeLine[0].color;
	m_edges = edges;

	m_highestFitness = 0.0;
	m_highestFitnessOverall = 0.0;
	m_minFitnessImprovement = 0.0;

	// Generate triangle checkpoints
	m_triangleCheckpoints = GenerateTriangleCheckpoints(m_edges, m_edgesPivot);

	// Set shape point count
	m_triangleCheckpointShape.setPointCount(m_triangleCheckpoints.back().size());

	// Add finish line and blocking edge
	m_edges.push_back({ m_edges.front()[0], m_edges[pivot][0] });
	m_edges.push_back({ m_edges.front()[1], m_edges[pivot][1] });

	// Erase continuity
	m_edges.erase(m_edges.begin() + pivot);
	m_edges.erase(m_edges.begin());
}

DrawableMap::DrawableMap(const DrawableMap& drawableMap) : 
	m_edgesPivot(drawableMap.m_edgesPivot)
{
	m_edgeLine[0].color = sf::Color::White;
	m_edgeLine[1].color = m_edgeLine[0].color;
	m_edges = drawableMap.m_edges;
	m_highestFitness = 0.0;
	m_highestFitnessOverall = 0.0;
	m_minFitnessImprovement = 0.0;
	m_triangleCheckpoints = drawableMap.m_triangleCheckpoints;
	m_triangleCheckpointShape.setPointCount(m_triangleCheckpoints.back().size());
}

DrawableMap::~DrawableMap()
{
}

void DrawableMap::Reset()
{
	for (size_t i = 0; i < m_fitnessVector.size(); ++i)
	{
		m_fitnessVector[i] = 0;
		m_previousFitnessVector[i] = 0.0;
		m_timers[i].Reset();
	}
}

void DrawableMap::Init(size_t size, double minFitnessImprovement)
{
	m_highestFitness = 0.0;
	m_highestFitnessOverall = 0.0;

	for (size_t i = 0; i < m_fitnessVector.size(); ++i)
	{
		m_fitnessVector[i] = 0.0;
		m_previousFitnessVector[i] = 0.0;
		m_timers[i].Reset();
	}

	m_fitnessVector.resize(size, 0.0);
	m_previousFitnessVector.resize(size, 0.0);
	m_timers.resize(size, StoppableTimer(1.0, std::numeric_limits<double>::max()));
	m_minFitnessImprovement = minFitnessImprovement;
}

void DrawableMap::Update(DrawableVehicleFactory& drawableVehicleFactory)
{
	size_t count = drawableVehicleFactory.size();
	for (size_t i = 0; i < count; ++i)
	{
		if (!drawableVehicleFactory[i]->IsActive())
			continue;

		m_timers[i].Update();
		for (auto& edge : m_edges)
		{
			if (DrawableMath::Intersect(edge, drawableVehicleFactory[i]->GetVertices()))
				drawableVehicleFactory[i]->SetInactive();
			else
				drawableVehicleFactory[i]->Detect(edge);
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

void DrawableMap::DrawCheckpoints()
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
}

void DrawableMap::Iterate(DrawableVehicleFactory& drawableVehicleFactory)
{
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
		m_fitnessVector[i] = CalculateFitness(drawableVehicleFactory[i]);

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end()); // Max: 100%
	auto value = *iterator;

	if (m_highestFitnessOverall < value)
		m_highestFitnessOverall = value;
	m_highestFitness = 0.0;

	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		m_fitnessVector[i] *= 100000;
		m_fitnessVector[i] += (1.0 / m_timers[i].GetValue()) * 10000;
	}
}

size_t DrawableMap::MarkLeader(DrawableVehicleFactory& drawableVehicleFactory)
{
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		drawableVehicleFactory[i]->SetFollowerColor();
		if (!drawableVehicleFactory[i]->IsActive())
		{
			m_fitnessVector[i] = 0.0;
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
	double meanRequiredFitness = 0.0;
	auto maxFitness = GetMaxFitness();
	for (size_t i = 0; i < drawableVehicleFactory.size(); ++i)
	{
		if (!drawableVehicleFactory[i]->IsActive())
			continue;
		
		m_fitnessVector[i] = CalculateFitness(drawableVehicleFactory[i]);
		auto requiredFitness = m_previousFitnessVector[i];
		requiredFitness += maxFitness * m_minFitnessImprovement;
		if (requiredFitness > m_fitnessVector[i])
			drawableVehicleFactory[i]->SetInactive();

		if (requiredFitness > m_fitnessVector[i])
			m_previousFitnessVector[i] = requiredFitness;
		else
			m_previousFitnessVector[i] = m_fitnessVector[i];

		++population;
		meanRequiredFitness += m_previousFitnessVector[i];
	}

	return std::pair(population, meanRequiredFitness / maxFitness / population);
}

const FitnessVector& DrawableMap::GetFitnessVector() const
{
	return m_fitnessVector;
}

Fitness DrawableMap::GetHighestFitness()
{
	return m_highestFitness / GetMaxFitness();
}

Fitness DrawableMap::GetHighestFitnessOverall()
{
	return m_highestFitnessOverall / GetMaxFitness();
}

Fitness DrawableMap::CalculateFitness(DrawableVehicle* drawableVehicle)
{
	Fitness fitness = GetMaxFitness();
	for (size_t i = 0; i < m_triangleCheckpoints.size(); ++i)
	{
		if (DrawableMath::Intersect(m_triangleCheckpoints[i], drawableVehicle->GetVertices()))
			fitness = Fitness(i);
	}

	return fitness;
}

TriangleVector DrawableMap::GenerateTriangleCheckpoints(const EdgeVector& edges, const size_t pivot)
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
	auto edgePrecedencesVector = GetEdgePrecedencesVector(edges, pivot, endPointsVector);

	// Generate triangle checkpoints
	return GetTriangleCheckpoints(edges, edgePrecedencesVector);
}

Fitness DrawableMap::GetMaxFitness()
{
	return Fitness(m_triangleCheckpoints.size()) + 1.0;
}

std::vector<std::pair<size_t, size_t>> DrawableMap::GetEndPoints(const EdgeVector& edges, const size_t pivot, const size_t index)
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

DrawableMap::EdgePrecedencesVector DrawableMap::GetEdgePrecedencesVector(const EdgeVector& edges, const size_t pivot, EndPointsVector& endPointsVector)
{
	EdgePrecedencesVector result(pivot, EdgePrecedences());

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
			for (size_t k = 0; k < result.size(); ++k)
			{
				if (k == i)
					continue;

				for (size_t c = 0; c < result[k].size(); ++c)
				{
					if (DrawableMath::IntersectNonCollinear(edge, result[k][c].first))
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
				result[i].push_back(std::make_pair(edge, endPointPrecedence));
		}
	}

	auto Compare = [&](const EdgePrecedence& a, const EdgePrecedence& b)
	{
		return a.second < b.second;
	};

	for (auto& edgePrecedences : result)
		std::sort(edgePrecedences.begin(), edgePrecedences.end(), Compare);

	return result;
}

TriangleVector DrawableMap::GetTriangleCheckpoints(const EdgeVector& edges, const EdgePrecedencesVector& edgePrecedencesVector)
{
	TriangleVector result;
	auto checkpointCount = edgePrecedencesVector.size();
	for (size_t i = 1; i < checkpointCount; ++i)
	{
		auto& edgePrecedences = edgePrecedencesVector[i];
		if (edgePrecedences.empty())
			return {};

		size_t lastIndex = edgePrecedences.size() - 1;
		for (size_t j = 0; j < lastIndex; ++j)
		{
			Triangle triangle = { edgePrecedences[j].first[0], edgePrecedences[j].first[1], edgePrecedences[j + 1].first[1] };
			result.push_back(triangle);
		}

		size_t nextIndex = (i + 1 >= checkpointCount) ? 0 : (i + 1);
		Triangle triangle = { edgePrecedences[lastIndex].first[0], edgePrecedences[lastIndex].first[1], edges[nextIndex][0] };
		result.push_back(triangle);
	}

	result.shrink_to_fit();

	return result;
}
