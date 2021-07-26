#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "DrawableMath.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager
{
	Line m_edgeLine;
	Line m_checkpointLine;
	EdgeVector m_edges;
	EdgeVector m_checkpoints;
	DrawableFinishLine m_finishLine;

public:
	DrawableManager(EdgeVector&& edges, Edge&& finishLine, EdgeVector&& checkpoints)
	{
		m_edgeLine[0].color = sf::Color::White;
		m_edgeLine[1].color = m_edgeLine[0].color;
		m_checkpointLine[0].color = sf::Color(0, 255, 0, 64);
		m_checkpointLine[1].color = m_checkpointLine[0].color;
		m_edges = std::move(edges);
		m_finishLine.set(finishLine);
		m_checkpoints = std::move(checkpoints);
	}

	~DrawableManager()
	{
	}

	inline void intersect(DrawableCarFactory& cars)
	{
		for (auto& car : cars)
		{
			if (!car.second)
				continue;

			if (Intersect(m_finishLine.m_edge, car.first->m_points))
			{

			}

			for (auto & edge : m_edges)
			{
				if (Intersect(edge, car.first->m_points))
				{
					car.second = false;
				}
				else
					car.first->detect(edge);
			}
		}
	}

	inline void draw()
	{
		m_finishLine.draw();
		for (const auto& i : m_edges)
		{
			m_edgeLine[0].position = i[0];
			m_edgeLine[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_edgeLine.data(), 2, sf::Lines);
		}

		for (const auto& i : m_checkpoints)
		{
			m_checkpointLine[0].position = i[0];
			m_checkpointLine[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_checkpointLine.data(), 2, sf::Lines);
		}
	}
};