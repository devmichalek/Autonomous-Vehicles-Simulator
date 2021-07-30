#pragma once
#include "DrawableManager.hpp"

const Edge& DrawableManager::getFinishLine() const
{
	return m_finishLine.get();
}

void DrawableManager::intersect(DetailedCarFactory& cars)
{
	for (auto& car : cars)
	{
		if (!car.second)
			continue;

		if (Intersect(m_finishLine.get(), car.first->getPoints()))
		{
			car.second = false;
		}

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

void DrawableManager::drawFinishLine()
{
	m_finishLine.draw();
}

void DrawableManager::drawEdges()
{
	for (const auto& i : m_edges)
	{
		m_edgeLine[0].position = i[0];
		m_edgeLine[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_edgeLine.data(), 2, sf::Lines);
	}
}
