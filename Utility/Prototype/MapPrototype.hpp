#pragma once
#include "DrawableMath.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class MapPrototype final
{
	EdgeVector m_edges;
	EdgeVector m_innerEdgesChain;
	EdgeVector m_outerEdgesChain;
	RectangleVector m_checkpoints;
	EdgeShape m_edgeShape;
	sf::ConvexShape m_checkpointShape;

public:

	MapPrototype(const EdgeVector& innerEdgesChain,
				 const EdgeVector& outerEdgesChain,
				 const RectangleVector& checkpoints) :
		m_innerEdgesChain(innerEdgesChain),
		m_outerEdgesChain(outerEdgesChain),
		m_checkpoints(checkpoints)
	{
		m_edgeShape[0].color = sf::Color::White;
		m_edgeShape[1].color = m_edgeShape[0].color;
		m_checkpointShape.setPointCount(checkpoints.back().size());
	}

	MapPrototype()
	{
		m_edgeShape[0].color = sf::Color::White;
		m_edgeShape[1].color = m_edgeShape[0].color;
		RectangleVector rectangle;
		m_checkpointShape.setPointCount(rectangle.size());
	}

	// Adds edge to the container
	inline void AddEdge(Edge edge)
	{
		m_edges.push_back(edge);
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

	// Returns inner edges chain
	inline const EdgeVector& GetInnerEdgesChain() const
	{
		return m_innerEdgesChain;
	}

	// Returns outer edges chain
	inline const EdgeVector& GetOuterEdgesChain() const
	{
		return m_outerEdgesChain;
	}

	// Returns number of edges
	inline size_t GetNumberOfEdges() const
	{
		return m_edges.size();
	}

	// Returns checkpoints
	inline const RectangleVector& GetCheckpoints() const
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