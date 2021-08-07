#pragma once
#include "DrawableCar.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableEdgeManager
{
	Line m_edgeLine;
	EdgeVector m_edges;

public:
	DrawableEdgeManager(EdgeVector&& edges)
	{
		m_edgeLine[0].color = sf::Color::White;
		m_edgeLine[1].color = m_edgeLine[0].color;
		m_edges = std::move(edges);
	}

	~DrawableEdgeManager()
	{
	}

	void intersect(DetailedCarFactory& cars);

	void drawEdges();
};