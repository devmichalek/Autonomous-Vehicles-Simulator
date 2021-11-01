#include "MapPrototype.hpp"
#include "CoreWindow.hpp"

bool MapPrototype::FindClosestPointOnIntersection(Edge edge, sf::Vector2f& point)
{
	const size_t size = m_edges.size() - 1;
	for (size_t i = 0; i < size; ++i)
	{
		sf::Vector2f intersectionPoint;
		if (DrawableMath::GetIntersectionPoint(m_edges[i], edge, intersectionPoint))
		{
			auto segment = DrawableMath::Distance(m_edges[i][0], intersectionPoint);
			auto length = DrawableMath::Distance(m_edges[i]);
			double percentage = segment / length;
			if (percentage > 0.01 && percentage < 0.5)
			{
				point = m_edges[i][0];
				return true;
			}
				
			if (percentage > 0.5 && percentage < 0.99)
			{
				point = m_edges[i][1];
				return true;
			}
		}
	}

	return false;
}

bool MapPrototype::FindClosestPointOnDistance(const sf::Vector2f& base, sf::Vector2f& point)
{
	// Find closest point to mouse position
	double distance = std::numeric_limits<double>::max();
	for (auto& edge : m_edges)
	{
		double beginningDistance = DrawableMath::Distance(base, edge[0]);
		if (beginningDistance < distance)
		{
			distance = beginningDistance;
			point = edge[0];
		}

		double endDistance = DrawableMath::Distance(base, edge[1]);
		if (endDistance < distance)
		{
			distance = endDistance;
			point = edge[1];
		}
	}

	// There will be always a closest point if there is at least one edge in container
	return !m_edges.empty();
}

bool MapPrototype::RemoveEdgesOnInterscetion(const Edge& edge)
{
	bool result = false; // True if any edge was removed
	size_t size = m_edges.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (DrawableMath::Intersect(m_edges[i], edge))
		{
			result = true;
			m_edges.erase(m_edges.begin() + i);
			--size;
			--i;
		}
	}

	return result;
}

void MapPrototype::DrawEdges()
{
	for (auto& edge : m_edges)
	{
		m_edgeShape[0].position = edge[0];
		m_edgeShape[1].position = edge[1];
		CoreWindow::Draw(m_edgeShape.data(), m_edgeShape.size(), sf::Lines);
	}

	for (auto& edge : m_innerEdgesChain)
	{
		m_edgeShape[0].position = edge[0];
		m_edgeShape[1].position = edge[1];
		CoreWindow::Draw(m_edgeShape.data(), m_edgeShape.size(), sf::Lines);
	}

	for (auto& edge : m_outerEdgesChain)
	{
		m_edgeShape[0].position = edge[0];
		m_edgeShape[1].position = edge[1];
		CoreWindow::Draw(m_edgeShape.data(), m_edgeShape.size(), sf::Lines);
	}
}

void MapPrototype::DrawCheckpoints()
{
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		sf::Uint8 red = 255 * (i % 3);
		sf::Uint8 green = 255 * ((i + 1) % 3);
		sf::Uint8 blue = 255 * ((i + 2) % 3);
		m_checkpointShape.setFillColor(sf::Color(red, green, blue, 96));
		for (size_t j = 0; j < m_checkpointShape.getPointCount(); ++j)
			m_checkpointShape.setPoint(j, m_checkpoints[i][j]);
		CoreWindow::Draw(m_checkpointShape);
	}
}