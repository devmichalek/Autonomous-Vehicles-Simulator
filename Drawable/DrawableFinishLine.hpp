#pragma once
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"

class DrawableFinishLine
{
	Line m_line;

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
		m_line[0].position = point;
	}

	inline sf::Vector2f getStartPoint()
	{
		return m_line[0].position;
	}

	inline void setEndPoint(sf::Vector2f point)
	{
		m_line[1].position = point;
	}

	inline sf::Vector2f getEndPoint()
	{
		return m_line[1].position;
	}

	inline void setPoints(Segment segment)
	{
		setStartPoint(segment[0]);
		setEndPoint(segment[1]);
	}

	inline void draw()
	{
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}
};