#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "DrawableMath.hpp"

class DrawableWallManager
{
	Line m_line;
	SegmentVector m_segments;

	// Check if segment intersects with car segments
	// This function does not work with collinear points!
	inline bool intersect(Segment& segment, RectanglePoints carPoints)
	{
		if (::intersect(segment[0], segment[1], carPoints[0], carPoints[1]))
			return true;

		if (::intersect(segment[0], segment[1], carPoints[1], carPoints[2]))
			return true;

		if (::intersect(segment[0], segment[1], carPoints[2], carPoints[3]))
			return true;

		if (::intersect(segment[0], segment[1], carPoints[3], carPoints[0]))
			return true;

		return false;
	}

public:
	DrawableWallManager(SegmentVector&& segments)
	{
		m_segments = std::move(segments);
	}

	~DrawableWallManager()
	{
	}

	inline void intersect(DrawableCarFactory& cars)
	{
		for (auto & segment : m_segments)
		{
			for (auto& car : cars)
			{
				if (!car.second)
					continue;

				if (intersect(segment, car.first->getPoints()))
				{

				}
			}
		}
	}

	inline void draw()
	{
		for (const auto& i : m_segments)
		{
			m_line[0].position = i[0];
			m_line[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		}
	}
};