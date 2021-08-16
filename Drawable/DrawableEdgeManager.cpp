#pragma once
#include "DrawableEdgeManager.hpp"

DrawableEdgeManager::DrawableEdgeManager(EdgeVector edges, size_t pivot)
{
	m_edgeLine[0].color = sf::Color::White;
	m_edgeLine[1].color = m_edgeLine[0].color;
	m_edges = std::move(edges);

	// Add finish line position
	Edge finishLineEdge;
	finishLineEdge[0] = m_edges.front()[0];
	finishLineEdge[1] = m_edges[pivot][0];
	m_edges.push_back(finishLineEdge);

	// Set blocking edge position
	Edge blockEdge;
	blockEdge[0] = m_edges.front()[1];
	blockEdge[1] = m_edges[pivot][1];
	m_edges.insert(m_edges.begin(), blockEdge);

	// Erase continuity
	m_edges.erase(m_edges.begin() + pivot + 1);
	m_edges.erase(m_edges.begin() + 1);
}

DrawableEdgeManager::~DrawableEdgeManager()
{
}

void DrawableEdgeManager::Intersect(DetailedCarFactory& cars)
{
	for (auto& car : cars)
	{
		if (!car.second)
			continue;

		for (auto& edge : m_edges)
		{
			if (::Intersect(edge, car.first->getPoints()))
			{
				car.second = false;
			}
			else
				car.first->detect(edge);
		}
	}
}

void DrawableEdgeManager::Draw()
{
	for (const auto& i : m_edges)
	{
		m_edgeLine[0].position = i[0];
		m_edgeLine[1].position = i[1];
		CoreWindow::GetRenderWindow().draw(m_edgeLine.data(), 2, sf::Lines);
	}
}
