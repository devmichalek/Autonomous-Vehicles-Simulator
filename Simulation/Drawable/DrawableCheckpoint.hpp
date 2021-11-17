#pragma once
#include "DrawableInterface.hpp"
#include "MathContext.hpp"
#include "CoreWindow.hpp"

class DrawableCheckpoint :
	public DrawableInterface
{
	static inline bool m_visibility = false;
	RectangleShape m_rectangleShape;

protected:

	DrawableCheckpoint(sf::Color color, Rectangle position)
	{
		for (size_t i = 0; i < m_rectangleShape.size(); ++i)
		{
			m_rectangleShape[i].color = color;
			m_rectangleShape[i].position = position[i];
		}
	}

public:

	// Draws checkpoint
	void Draw()
	{
		if (DrawableCheckpoint::m_visibility)
			CoreWindow::Draw(m_rectangleShape.data(), m_rectangleShape.size(), sf::Quads);
	}

	// Sets visibility of checkpoints
	inline static void SetVisibility(bool visibility)
	{
		m_visibility = visibility;
	}
};