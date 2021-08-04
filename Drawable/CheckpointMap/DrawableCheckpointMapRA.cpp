#include "DrawableCheckpointMapRA.hpp"
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"

DrawableCheckpointMapRA::DrawableCheckpointMapRA(const EdgeVector& edges, const size_t pivot, const Edge& finishLine)
{
	const float checkpointLength = float(CoreWindow::getSize().x);
	const float checkpointGap = float((CoreWindow::getSize().x / 30.0f) / 3);

	// For all inner edges from the edge sequence
	for (size_t i = 0; i < pivot; ++i)
	{
		auto edgeLength = Distance(edges[i]) - 1;
		auto p1 = edges[i][0];
		auto p2 = edges[i][1];
		float angle = (float)DifferenceVectorAngle(p1, p2);

		const float checkpointAngle = angle - 90; // 90 degrees
		size_t checkpointCount = static_cast<size_t>(edgeLength / checkpointGap) + 1;

		for (float gap = 0; checkpointCount-- > 0; gap -= checkpointGap)
		{
			sf::Vector2f startPoint = GetEndPoint(p1, angle, gap);
			startPoint = GetEndPoint(startPoint, checkpointAngle, 1.0);
			sf::Vector2f endPoint = GetEndPoint(startPoint, checkpointAngle, checkpointLength);
			m_checkpoints.push_back({ startPoint, endPoint });
		}
	}

	m_innerMax = m_checkpoints.size();

	// For all outer edges from the edge sequence
	for (size_t i = pivot; i < edges.size(); ++i)
	{
		auto edgeLength = Distance(edges[i]) - 1;
		auto p1 = edges[i][0];
		auto p2 = edges[i][1];
		float angle = (float)DifferenceVectorAngle(p1, p2);

		const float checkpointAngle = angle + 90; // 90 degrees
		size_t checkpointCount = static_cast<size_t>(edgeLength / checkpointGap) + 1;

		for (float gap = 0; checkpointCount-- > 0; gap -= checkpointGap)
		{
			sf::Vector2f startPoint = GetEndPoint(p1, angle, gap);
			startPoint = GetEndPoint(startPoint, checkpointAngle, 1.0);
			sf::Vector2f endPoint = GetEndPoint(startPoint, checkpointAngle, checkpointLength);
			m_checkpoints.push_back({ startPoint, endPoint });
		}
	}

	// Check if particular inner checkpoint does not intersect with a edge
	// If there is intersection then shorten the checkpoint
	sf::Vector2f intersectionPoint;
	for (size_t i = 0; i < m_innerMax; ++i)
	{
		for (auto& j : edges)
		{
			if (GetIntersectionPoint(m_checkpoints[i], j, intersectionPoint))
			{
				m_checkpoints[i][1] = intersectionPoint;
			}
		}

		if (GetIntersectionPoint(m_checkpoints[i], finishLine, intersectionPoint))
		{
			m_checkpoints[i][1] = intersectionPoint;
		}
	}

	// Check if particular outer checkpoint does not intersect with a edge
	// If there is intersection then shorten the checkpoint
	for (size_t i = m_innerMax; i < m_checkpoints.size(); ++i)
	{
		for (auto& j : edges)
		{
			if (GetIntersectionPoint(m_checkpoints[i], j, intersectionPoint))
			{
				m_checkpoints[i][1] = intersectionPoint;
			}
		}

		if (GetIntersectionPoint(m_checkpoints[i], finishLine, intersectionPoint))
		{
			m_checkpoints[i][1] = intersectionPoint;
		}
	}

	m_line[0].color = sf::Color(0, 255, 0, 64);
	m_line[1].color = m_line[0].color;
}

void DrawableCheckpointMapRA::draw()
{
	for (const auto& i : m_checkpoints)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}
}

Fitness DrawableCheckpointMapRA::calculateFitness(DetailedCar& car, const Edge& finishLine)
{
	if (Intersect(finishLine, car.first->getPoints()))
		return Fitness(-1);

	size_t i = 0;
	Fitness innerFitness = 0;
	for (; i < m_innerMax; ++i)
	{
		if (Intersect(m_checkpoints[i], car.first->getPoints()))
			innerFitness = Fitness(i);
	}

	Fitness outerFitness = 0;
	for (i = m_innerMax; i < m_checkpoints.size(); ++i)
	{
		if (Intersect(m_checkpoints[i], car.first->getPoints()))
			outerFitness = Fitness(i - m_innerMax);
	}

	if (innerFitness && outerFitness)
		return (innerFitness + outerFitness) / 2;

	double percentage;
	if (innerFitness)
		percentage = double(innerFitness) / m_innerMax;
	else
		percentage = double(outerFitness) / (m_checkpoints.size() - m_innerMax);
	return Fitness(percentage * getMaxFitness());
}

Fitness DrawableCheckpointMapRA::getMaxFitness()
{
	return Fitness(double(m_checkpoints.size()) / 2);
}
