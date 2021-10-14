#pragma once
#include "DrawableInterface.hpp"
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"

class DrawableCheckpoint :
	public DrawableInterface
{
	static inline bool m_visibility = false;
	TriangleShape m_triangleShape;

protected:

	DrawableCheckpoint(sf::Color color, Triangle position)
	{
		for (size_t i = 0; i < m_triangleShape.size(); ++i)
		{
			m_triangleShape[i].color = color;
			m_triangleShape[i].position = position[i];
		}
	}

public:

	// Draws checkpoint
	void Draw()
	{
		if (DrawableCheckpoint::m_visibility)
			CoreWindow::GetRenderWindow().draw(m_triangleShape.data(), m_triangleShape.size(), sf::Triangles);
	}

	// Sets visibility of checkpoints
	inline static void SetVisibility(bool visibility)
	{
		m_visibility = visibility;
	}
};