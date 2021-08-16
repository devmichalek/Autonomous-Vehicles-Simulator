#pragma once
#include "DrawableCheckpointMap.hpp"
#include "DrawableCar.hpp"

Fitness DrawableCheckpointMap::getMaxFitness()
{
	return Fitness(m_checkpoints.size());
}

Fitness DrawableCheckpointMap::calculateFitness(DetailedCar& car)
{
	Fitness fitness = 0;
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		if (IsCarInsideTriangle(m_checkpoints[i], car.first->getPoints()))
			fitness = Fitness(i + 1);
	}

	return fitness;
}

DrawableCheckpointMap::DrawableCheckpointMap(const EdgeVector& edges, const size_t pivot)
{
	m_highestFitness = 0;
	m_highestFitnessOverall = 0;

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
				// Check if there is no intersection with any edge
				if (k == i || k == j)
					continue;

				if (Intersect(edge, edges[k]))
				{
					intersection = true;
					break;
				}
			}

			// There was no intersection
			if (!intersection)
			{
				// Calculate distance
				auto distance = Distance(edge);
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
					if (Intersect(std::get<0>(checkpoints[k][c]), edge))
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
				auto angle = DifferenceVectorAngle(edge[0], edge[1]);
				angle = CastAtan2ToFullAngle(angle);

				// Push back
				checkpoints[i].push_back(std::make_pair(edge, angle));
			}
		}
	}

	// Calculate base angles
	std::vector<double> angleBordersVector(pivot);
	for (size_t i = 1; i < pivot; ++i)
		angleBordersVector[i] = CastAtan2ToFullAngle(DifferenceVectorAngle(edges[i - 1][1], edges[i - 1][0]));
	angleBordersVector[0] = CastAtan2ToFullAngle(DifferenceVectorAngle(edges[pivot - 1][1], edges[pivot - 1][0]));

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

	// Add checkpoints, make triangles
	size_t checkpointCount = checkpoints.size();
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

	// Erase continuity
	m_checkpoints.erase(m_checkpoints.begin());
	m_checkpoints.erase(m_checkpoints.begin());

	// Set shape point count
	m_shape.setPointCount(TRIANGLE_NUMBER_OF_POINTS);
}

DrawableCheckpointMap::~DrawableCheckpointMap()
{
}

void DrawableCheckpointMap::draw()
{
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		for (size_t j = 0; j < TRIANGLE_NUMBER_OF_POINTS; ++j)
			m_shape.setPoint(j, m_checkpoints[i][j]);

		sf::Uint8 red = 255 * (i % 3);
		sf::Uint8 green = 255 * ((i + 1) % 3);
		sf::Uint8 blue = 255 * ((i + 2) % 3);
		m_shape.setFillColor(sf::Color(red, green, blue, 48));
		CoreWindow::GetRenderWindow().draw(m_shape);
	}
}

void DrawableCheckpointMap::iterate(DetailedCarFactory& factory)
{
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		m_fitnessVector[i] = calculateFitness(factory[i]);
		m_fitnessVector[i] += static_cast<Fitness>(double(maxFitness) / m_timers[i].Value());
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;
}

size_t DrawableCheckpointMap::markLeader(DetailedCarFactory& factory)
{
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		factory[i].first->setFollowerColor();
		if (!factory[i].second)
		{
			m_fitnessVector[i] = 0;
			continue;
		}
		m_fitnessVector[i] = calculateFitness(factory[i]);
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;

	size_t index = std::distance(m_fitnessVector.begin(), iterator);
	factory[index].first->setLeaderColor();
	return index;
}

void DrawableCheckpointMap::punish(DetailedCarFactory& factory)
{
	// Check if car has made improvement
	// If car has made improvement then it is not punished
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		if (!factory[i].second)
			continue;
		m_fitnessVector[i] = calculateFitness(factory[i]);
		Fitness requiredFitness = m_previousFitnessVector[i];
		requiredFitness += static_cast<Fitness>(double(maxFitness) * m_minFitnessImprovement);
		if (requiredFitness > m_fitnessVector[i])
			factory[i].second = false;
		m_previousFitnessVector[i] = m_fitnessVector[i];
	}
}

void DrawableCheckpointMap::reset()
{
	for (size_t i = 0; i < m_fitnessVector.size(); ++i)
	{
		m_fitnessVector[i] = 0;
		m_previousFitnessVector[i] = 0;
		m_timers[i].Reset();
	}
}

void DrawableCheckpointMap::restart(size_t size, double minFitnessImprovement)
{
	m_fitnessVector.resize(size, 0);
	m_previousFitnessVector.resize(size, 0);
	m_timers.resize(size, StoppableTimer(0.0, std::numeric_limits<double>::max()));
	m_minFitnessImprovement = minFitnessImprovement;
}

void DrawableCheckpointMap::incrementTimers()
{
	for (auto& timer : m_timers)
		timer.Increment();
}

const FitnessVector& DrawableCheckpointMap::getFitnessVector() const
{
	return m_fitnessVector;
}

const Fitness& DrawableCheckpointMap::getHighestFitness() const
{
	return m_highestFitness;
}

const Fitness& DrawableCheckpointMap::getHighestFitnessOverall() const
{
	return m_highestFitnessOverall;
}
