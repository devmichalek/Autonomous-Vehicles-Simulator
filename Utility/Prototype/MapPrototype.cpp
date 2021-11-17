#include "MapPrototype.hpp"
#include "CoreWindow.hpp"

bool MapPrototype::FindEndPoint(sf::Vector2f point, const EdgeVector& edges, size_t& index) const
{
	for (size_t i = 0; i < edges.size(); ++i)
	{
		if (edges[i][1] == point)
		{
			index = i + 1;
			return true;
		}
	}

	return false;
}

bool MapPrototype::GetIntersectionPoint(const Edge& a, const Edge& b, sf::Vector2f& point) const
{
	sf::Vector2f intersectionPoint;
	if (MathContext::GetIntersectionPoint(a, b, intersectionPoint))
	{
		auto segment = MathContext::Distance(a[0], intersectionPoint);
		auto length = MathContext::Distance(a);
		double percentage = segment / length;
		if (percentage > 0.01 && percentage < 0.5)
		{
			point = a[0];
			return true;
		}

		if (percentage > 0.5 && percentage < 0.99)
		{
			point = a[1];
			return true;
		}
	}

	return false;
}

bool MapPrototype::AddInnerEdge(Edge edge)
{
	if (IsInnerEdgesChainCompleted())
		return false;

	if (m_innerEdgesChain.empty())
	{
		m_innerEdgesChain.push_back(edge);
		m_innerEdgesChainCompleted = MathContext::IsEdgesChain(m_innerEdgesChain);
		m_outerEdgesGaps.push_back(true);
		m_outerEdgesChain.push_back({});
		return true;
	}

	// Find end point
	for (size_t i = 0; i < m_innerEdgesChain.size(); ++i)
	{
		if (m_innerEdgesChain[i][1] == edge[0])
		{
			size_t index = i + 1;
			m_innerEdgesChain.insert(m_innerEdgesChain.begin() + index, edge);
			m_innerEdgesChainCompleted = MathContext::IsEdgesChain(m_innerEdgesChain);

			if (m_innerEdgesChainCompleted)
			{
				m_outerEdgesGaps.resize(m_innerEdgesChain.size(), true);
				m_outerEdgesChain.resize(m_innerEdgesChain.size());
			}
			else
			{
				m_outerEdgesGaps.push_back(true);
				m_outerEdgesChain.push_back({});
			}

			return true;
		}
	}

	return false;
}

bool MapPrototype::AddOuterEdge(Edge edge)
{
	if (!IsInnerEdgesChainCompleted())
		return false;

	if (IsOuterEdgesChainCompleted())
		return false;

	// Find end point
	for (size_t i = 0; i < m_outerEdgesChain.size(); ++i)
	{
		if (m_outerEdgesGaps[i])
		{
			++m_numberOfOuterEdges;
			m_outerEdgesGaps[i] = false;
			m_outerEdgesChain[i] = edge;
			m_outerEdgesChainCompleted = MathContext::IsEdgesChain(m_outerEdgesChain);
			return true;
		}
	}

	return false;
}

void MapPrototype::SetEdgesChains(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain)
{
	m_innerEdgesChain = innerEdgesChain;
	m_innerEdgesChainCompleted = MathContext::IsEdgesChain(m_innerEdgesChain);

	if (m_innerEdgesChainCompleted)
	{
		m_outerEdgesChain = outerEdgesChain;
		m_outerEdgesChainCompleted = MathContext::IsEdgesChain(m_outerEdgesChain);

		if (m_outerEdgesChainCompleted)
		{
			std::fill(m_outerEdgesGaps.begin(), m_outerEdgesGaps.end(), false);
			m_outerEdgesGaps.resize(m_innerEdgesChain.size(), false);
			m_numberOfOuterEdges = m_outerEdgesChain.size();
		}
		else
		{
			m_innerEdgesChain.clear();
			m_outerEdgesChain.clear();
			m_numberOfOuterEdges = 0;
		}
	}
	else
		m_innerEdgesChain.clear();
}

void MapPrototype::CalculateProperties()
{
	if (!m_innerEdgesChain.empty())
	{
		float left = m_innerEdgesChain.front()[0].x;
		float right = left;
		float top = m_innerEdgesChain.front()[0].y;
		float down = top;

		auto CalculateInternal = [&](const Edge& edge)
		{
			const std::pair<float, float> horizontal = (edge[0].x < edge[1].x) ? std::pair(edge[0].x, edge[1].x) : std::pair(edge[1].x, edge[0].x);
			const std::pair<float, float> vertical = (edge[0].y < edge[1].y) ? std::pair(edge[0].y, edge[1].y) : std::pair(edge[1].y, edge[0].y);
			if (left > horizontal.first)
				left = horizontal.first;
			if (right < horizontal.second)
				right = horizontal.second;
			if (top > vertical.first)
				top = vertical.first;
			if (down < vertical.second)
				down = vertical.second;
		};

		for (const auto& edge : m_innerEdgesChain)
			CalculateInternal(edge);

		for (size_t i = 0; i < m_outerEdgesChain.size(); ++i)
		{
			if (!m_outerEdgesGaps[i])
				CalculateInternal(m_outerEdgesChain[i]);
		}

		m_size.x = std::fabs(right - left);
		m_size.y = std::fabs(down - top);
		m_center.x = left + (m_size.x / 2.f);
		m_center.y = top + (m_size.y / 2.f);
	}
}

bool MapPrototype::FindClosestPointOnIntersection(Edge edge, sf::Vector2f& point) const
{
	if (!IsInnerEdgesChainCompleted())
	{
		for (size_t i = 0; i < m_innerEdgesChain.size(); ++i)
		{
			if (GetIntersectionPoint(m_innerEdgesChain[i], edge, point))
				return true;
		}

		return false;
	}

	if (!IsOuterEdgesChainCompleted())
	{
		for (size_t i = 0; i < m_outerEdgesChain.size(); ++i)
		{
			if (m_outerEdgesGaps[i])
				continue;

			if (GetIntersectionPoint(m_outerEdgesChain[i], edge, point))
				return true;
		}
	}

	return false;
}

bool MapPrototype::FindClosestPointOnDistance(const sf::Vector2f& base, sf::Vector2f& point) const
{
	if (!IsInnerEdgesChainCompleted())
	{
		// Find closest point to mouse position
		double distance = std::numeric_limits<double>::max();
		const size_t numberOfEdges = m_innerEdgesChain.size();
		for (size_t i = 0; i < numberOfEdges; ++i)
		{
			const size_t nextIndex = (i + 1) % numberOfEdges;
			if (m_innerEdgesChain[i][1] == m_innerEdgesChain[nextIndex][0])
				continue; // There is no gap, continue

			double endDistance = MathContext::Distance(base, m_innerEdgesChain[i][1]);
			if (endDistance < distance)
			{
				distance = endDistance;
				point = m_innerEdgesChain[i][1];
			}
		}

		// There will be always a closest point if there is at least one edge in container
		return !m_innerEdgesChain.empty();
	}

	if (!IsOuterEdgesChainCompleted())
	{
		// Find closest point to mouse position
		bool result = false;
		double distance = std::numeric_limits<double>::max();
		const size_t numberOfEdges = m_outerEdgesChain.size();
		for (size_t i = 0; i < numberOfEdges; ++i)
		{
			const size_t nextIndex = (i + 1) % numberOfEdges;
			if (!m_outerEdgesGaps[nextIndex] && m_outerEdgesChain[i][1] == m_outerEdgesChain[nextIndex][0])
				continue; // There is no gap, continue

			if (m_outerEdgesGaps[i])
				continue;

			double endDistance = MathContext::Distance(base, m_outerEdgesChain[i][1]);
			if (endDistance < distance)
			{
				distance = endDistance;
				point = m_outerEdgesChain[i][1];
				result = true;
			}
		}

		return result;
	}

	return false;
}

bool MapPrototype::RemoveEdgesOnIntersection(const Edge& edge)
{
	size_t size = m_innerEdgesChain.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (MathContext::Intersect(m_innerEdgesChain[i], edge))
		{
			// Remove inner edge
			m_innerEdgesChain.erase(m_innerEdgesChain.begin() + i);
			--size;

			// Do not decrement counter, continue removing inner edges
			for (; i < size; ++i)
			{
				if (MathContext::Intersect(m_innerEdgesChain[i], edge))
				{
					m_innerEdgesChain.erase(m_innerEdgesChain.begin() + i);
					--size;
					--i;
				}
			}

			// If any inner edge was removed outer edges does not make sense
			m_numberOfOuterEdges = 0;
			std::fill(m_outerEdgesGaps.begin(), m_outerEdgesGaps.end(), true);
			m_outerEdgesGaps.resize(size, true);
			m_outerEdgesChain.resize(size);
			m_innerEdgesChainCompleted = false;
			m_outerEdgesChainCompleted = false;
			return true;
		}
	}
	
	for (size_t i = 0; i < m_outerEdgesGaps.size(); ++i)
	{
		if (m_outerEdgesGaps[i])
			continue;

		if (MathContext::Intersect(m_outerEdgesChain[i], edge))
		{
			// Mark gap
			--m_numberOfOuterEdges;
			m_outerEdgesGaps[i] = true;

			// Continue removing outer edges
			for (++i; i < size; ++i)
			{
				if (MathContext::Intersect(m_outerEdgesChain[i], edge))
				{
					--m_numberOfOuterEdges;
					m_outerEdgesGaps[i] = true;
				}
			}

			m_outerEdgesChainCompleted = false;
			return true;
		}
	}

	return false;
}

void MapPrototype::DrawEdges()
{
	for (size_t i = 0; i < m_innerEdgesChain.size(); ++i)
	{
		if (m_outerEdgesGaps[i])
		{
			m_markedEdgeShape[0].position = m_innerEdgesChain[i][0];
			m_markedEdgeShape[1].position = m_innerEdgesChain[i][1];
			CoreWindow::Draw(m_markedEdgeShape.data(), m_markedEdgeShape.size(), sf::Lines);
		}
		else
		{
			m_edgeShape[0].position = m_innerEdgesChain[i][0];
			m_edgeShape[1].position = m_innerEdgesChain[i][1];
			CoreWindow::Draw(m_edgeShape.data(), m_edgeShape.size(), sf::Lines);

			m_edgeShape[0].position = m_outerEdgesChain[i][0];
			m_edgeShape[1].position = m_outerEdgesChain[i][1];
			CoreWindow::Draw(m_edgeShape.data(), m_edgeShape.size(), sf::Lines);
		}
	}
}

void MapPrototype::DrawCheckpoints()
{
	for (size_t i = 0; i < m_checkpoints.size(); ++i)
	{
		sf::Uint8 red = ColorContext::MaxChannelValue * (i % 3);
		sf::Uint8 green = ColorContext::MaxChannelValue * ((i + 1) % 3);
		sf::Uint8 blue = ColorContext::MaxChannelValue * ((i + 2) % 3);
		m_checkpointShape.setFillColor(ColorContext::Create(red, green, blue, 96));
		for (size_t j = 0; j < m_checkpointShape.getPointCount(); ++j)
			m_checkpointShape.setPoint(j, m_checkpoints[i][j]);
		CoreWindow::Draw(m_checkpointShape);
	}
}

bool MapPrototype::IsCollision(const VehiclePrototype* vehiclePrototype)
{
	if (!m_innerEdgesChainCompleted || !m_outerEdgesChainCompleted)
		return false;

	if (!vehiclePrototype)
		return false;

	std::vector<sf::Vector2f> realPoints = vehiclePrototype->GetBodyPoints();
	for (auto& realPoint : realPoints)
		realPoint += vehiclePrototype->GetCenter();

	const size_t numberOfPoints = realPoints.size();
	for (size_t i = 0; i < m_numberOfOuterEdges; ++i)
	{
		for (size_t j = 0; j < numberOfPoints; ++j)
		{
			const size_t nextIndex = (j + 1) % numberOfPoints;
			if (MathContext::Intersect(m_innerEdgesChain[i], realPoints[j], realPoints[nextIndex]))
				return true;

			if (MathContext::Intersect(m_outerEdgesChain[i], realPoints[j], realPoints[nextIndex]))
				return true;
		}
	}

	return false;
}
