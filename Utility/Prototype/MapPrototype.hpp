#pragma once
#include "DrawableMath.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class MapPrototype final
{
	EdgeVector m_edges;
	TriangleVector m_checkpoints;
	const bool m_clockwise;
	EdgeShape m_edgeShape;
	sf::ConvexShape m_checkpointShape;

public:

	MapPrototype(const EdgeVector& edges,
				 const TriangleVector& checkpoints,
				 const bool clockwise) :
		m_edges(edges),
		m_checkpoints(checkpoints),
		m_clockwise(clockwise)
	{
		m_edgeShape[0].color = sf::Color::White;
		m_edgeShape[1].color = m_edgeShape[0].color;
		m_checkpointShape.setPointCount(checkpoints.back().size());
	}

	MapPrototype() :
		m_clockwise(false)
	{
		m_edgeShape[0].color = sf::Color::White;
		m_edgeShape[1].color = m_edgeShape[0].color;
		Triangle triangle;
		m_checkpointShape.setPointCount(triangle.size());
	}

	// Adds edge to the container
	inline void AddEdge(Edge edge)
	{
		m_edges.push_back(edge);
	}

	// Inserts edge to the specific place in container
	inline void InsertEdge(Edge edge, size_t index)
	{
		if (index < m_edges.size())
			m_edges.insert(m_edges.begin() + index, edge);
		else
			AddEdge(edge);
	}

	// Sets edges
	inline void SetEdges(const EdgeVector& edges)
	{
		m_edges = edges;
	}

	// Returns edges
	inline const EdgeVector& GetEdges() const
	{
		return m_edges;
	}

	// Returns number of edges
	inline size_t GetNumberOfEdges() const
	{
		return m_edges.size();
	}

	// Returns checkpoints
	inline const TriangleVector& GetCheckpoints() const
	{
		return m_checkpoints;
	}

	// Returns number of checkpoints
	inline size_t GetNumberOfCheckpoints()
	{
		return m_checkpoints.size();
	}

	// Returns true if there are no edges and no checkpoints
	inline bool IsEmpty()
	{
		return m_edges.empty() && m_checkpoints.empty();
	}

	// Returns true if order of edges is clockwise-like
	inline bool IsClockwise()
	{
		return m_clockwise;
	}

	// Finds closest point based on intersection
	bool FindClosestPointOnIntersection(Edge edge, sf::Vector2f& point);

	// Finds closest point based on distance
	bool FindClosestPointOnDistance(const sf::Vector2f& base, sf::Vector2f& point);

	// Returns true if any edge was removed from container
	bool RemoveEdgesOnInterscetion(const Edge& edge);

	// Draws edges in unoptimized manner
	void DrawEdges();

	// Draws checkpoints in unoptimized manner
	void DrawCheckpoints();
};