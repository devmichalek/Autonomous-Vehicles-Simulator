#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "DrawableMath.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager
{
	Line m_line;
	WallVector m_walls;
	DrawableFinishLine m_finishLine;

public:
	DrawableManager(WallVector&& walls, Wall&& finishLine)
	{
		m_walls = std::move(walls);
		m_finishLine.set(finishLine);
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

			if (Intersect(m_finishLine.m_wall, car.first->m_points))
			{

			}

			for (auto & wall : m_walls)
			{
				if (Intersect(wall, car.first->m_points))
				{
					car.second = false;
				}
				else
					car.first->detect(wall);
			}
		}
	}

	inline void draw()
	{
		m_finishLine.draw();
		for (const auto& i : m_walls)
		{
			m_line[0].position = i[0];
			m_line[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		}
	}
};