#pragma once
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"
#include "StoppableTimer.hpp"
#include "Genetic.hpp"

class DrawableManager
{
	Line m_edgeLine;
	EdgeVector m_edges;
	DrawableFinishLine m_finishLine;

public:
	DrawableManager(EdgeVector&& edges, Edge&& finishLine)
	{
		m_edgeLine[0].color = sf::Color::White;
		m_edgeLine[1].color = m_edgeLine[0].color;
		m_edges = std::move(edges);
		m_finishLine.set(finishLine);
	}

	~DrawableManager()
	{
	}

	const Edge& getFinishLine() const;

	void intersect(DetailedCarFactory& cars);

	void drawFinishLine();

	void drawEdges();
};