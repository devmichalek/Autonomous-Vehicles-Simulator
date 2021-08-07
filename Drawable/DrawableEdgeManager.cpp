#pragma once
#include "DrawableEdgeManager.hpp"

void DrawableEdgeManager::intersect(DetailedCarFactory& cars)
{
	for (auto& car : cars)
	{
		if (!car.second)
			continue;

		for (auto& edge : m_edges)
		{
			if (Intersect(edge, car.first->getPoints()))
			{
				car.second = false;
			}
			else
				car.first->detect(edge);
		}
	}
}

void DrawableEdgeManager::drawEdges()
{
	for (const auto& i : m_edges)
	{
		m_edgeLine[0].position = i[0];
		m_edgeLine[1].position = i[1];
		CoreWindow::GetRenderWindow().draw(m_edgeLine.data(), 2, sf::Lines);
	}
}
