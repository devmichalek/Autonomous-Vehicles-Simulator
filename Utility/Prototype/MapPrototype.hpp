#pragma once
#include "MathContext.hpp"
#include "ColorContext.hpp"
#include <SFML/Graphics/ConvexShape.hpp>

class MapPrototype final
{
	EdgeVector m_innerEdgesChain;
	EdgeVector m_outerEdgesChain;
	RectangleVector m_checkpoints;
	bool m_innerEdgesChainCompleted;
	bool m_outerEdgesChainCompleted;
	std::vector<bool> m_outerEdgesGaps;
	EdgeShape m_edgeShape;
	EdgeShape m_markedEdgeShape;
	sf::ConvexShape m_checkpointShape;
	sf::Vector2f m_center;
	sf::Vector2f m_size;

	// Searches for possible end point
	bool FindEndPoint(sf::Vector2f point, const EdgeVector& edges, size_t& index) const;

	// Return true if there is intersection and updates point value
	bool GetIntersectionPoint(const Edge& a, const Edge& b, sf::Vector2f& point) const;

public:

	// Map protype constructor with correct data
	MapPrototype(const EdgeVector& innerEdgesChain,
				 const EdgeVector& outerEdgesChain,
				 const RectangleVector& checkpoints) :
		m_checkpoints(checkpoints),
		m_center(0.f, 0.f),
		m_size(0.f, 0.f)
	{
		SetEdgesChains(innerEdgesChain, outerEdgesChain);
		m_edgeShape[0].color = m_edgeShape[1].color = ColorContext::EdgeDefault;
		m_markedEdgeShape[0].color = m_markedEdgeShape[1].color = ColorContext::EdgeMarked;
		m_checkpointShape.setPointCount(checkpoints.back().size());
	}

	MapPrototype() : 
		m_innerEdgesChainCompleted(false),
		m_outerEdgesChainCompleted(false),
		m_center(0.f, 0.f),
		m_size(0.f, 0.f)
	{
		m_edgeShape[0].color = m_edgeShape[1].color = ColorContext::EdgeDefault;
		m_markedEdgeShape[0].color = m_markedEdgeShape[1].color = ColorContext::EdgeMarked;
		RectangleVector rectangle;
		m_checkpointShape.setPointCount(rectangle.size());
	}

	// Adds inner edge to the inner edges chain container, returns true in case of success
	bool AddInnerEdge(Edge edge);

	// Adds outer edge to the outer edges chain container, returns true in case of success
	bool AddOuterEdge(Edge edge);

	// Sets edges chains
	void SetEdgesChains(const EdgeVector& innerEdgesChain, const EdgeVector& outerEdgesChain);

	// Returns true if inner edges chain is completed
	inline bool IsInnerEdgesChainCompleted() const
	{
		return m_innerEdgesChainCompleted;
	}

	// Returns true if outer edges chain is completed
	inline bool IsOuterEdgesChainCompleted() const
	{
		return m_outerEdgesChainCompleted;
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

	// Returns number of inner edges
	inline size_t GetNumberOfInnerEdges() const
	{
		return m_innerEdgesChain.size();
	}

	// Returns number of outer edges
	inline size_t GetNumberOfOuterEdges() const
	{
		return m_outerEdgesChain.size();
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

	// Calculates map center and map size
	void CalculateProperties();

	// Returns map center
	const sf::Vector2f& GetCenter() const
	{
		return m_center;
	}

	// Returns map size
	const sf::Vector2f& GetSize() const
	{
		return m_size;
	}

	// Return true if there is closest intersection point and updates value
	bool FindClosestPointOnIntersection(Edge edge, sf::Vector2f& point) const;

	// Finds closest point based on distance
	bool FindClosestPointOnDistance(const sf::Vector2f& base, sf::Vector2f& point) const;

	// Returns true if any edge was removed from container
	bool RemoveEdgesOnIntersection(const Edge& edge);

	// Draws edges in unoptimized manner
	void DrawEdges();

	// Draws checkpoints in unoptimized manner
	void DrawCheckpoints();
};