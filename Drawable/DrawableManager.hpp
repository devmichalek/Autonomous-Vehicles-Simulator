#pragma once
#include "DrawableCar.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableManager
{
	Line m_edgeLine;
	EdgeVector m_edges;
	const size_t m_finishLineIndex;

public:
	DrawableManager(EdgeVector&& edges) :
		m_finishLineIndex(edges.size() - 1)
	{
		m_edgeLine[0].color = sf::Color::White;
		m_edgeLine[1].color = m_edgeLine[0].color;
		m_edges = std::move(edges);
	}

	~DrawableManager()
	{
	}

	const Edge& getFinishLine() const;

	void intersect(DetailedCarFactory& cars);

	void drawFinishLine();

	void drawEdges();
};