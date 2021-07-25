#pragma once
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"

class DrawableFinishLine
{
	Line m_line;
	Wall m_wall;
	friend class DrawableManager;

public:
	DrawableFinishLine()
	{
		m_line[0].color = sf::Color(255, 255, 0, 128);
		m_line[1].color = m_line[0].color;
	}

	~DrawableFinishLine()
	{
	}

	inline void setStartPoint(sf::Vector2f point)
	{
		m_wall[0] = point;
	}

	inline sf::Vector2f getStartPoint()
	{
		return m_wall[0];
	}

	inline void setEndPoint(sf::Vector2f point)
	{
		m_wall[1] = point;
	}

	inline sf::Vector2f getEndPoint()
	{
		return m_wall[1];
	}

	inline void set(Wall wall)
	{
		m_wall = wall;
	}

	inline void draw()
	{
		m_line[0].position = m_wall[0];
		m_line[1].position = m_wall[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}
};