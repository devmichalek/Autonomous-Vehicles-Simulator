#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "DrawableMath.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager
{
	Line m_line;
	SegmentVector m_segments;
	DrawableFinishLine m_finishLine;

	// Check if segment intersects with car segments
	// This function does not work with collinear points!
	inline bool intersect(Segment& segment, CarPoints& carPoints)
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
	DrawableManager(SegmentVector&& segments, Segment&& finishLine)
	{
		m_segments = std::move(segments);
		m_finishLine.setPoints(finishLine);
	}

	~DrawableManager()
	{
	}

	inline void intersect(DrawableCarFactory& cars)
	{
		sf::Vector2f intersectionPoint;
		for (auto & segment : m_segments)
		{
			for (auto& car : cars)
			{
				if (!car.second)
					continue;

				if (intersect(segment, car.first->getPoints()))
				{
					car.second = false;
				}
				else
				{
					auto& beams = car.first->getBeams();
					for (auto& beam : beams)
					{
						if (::intersect(segment, beam, intersectionPoint))
						{
							beam[1] = intersectionPoint;
						}
					}
				}
			}
		}
	}

	inline void draw()
	{
		m_finishLine.draw();
		for (const auto& i : m_segments)
		{
			m_line[0].position = i[0];
			m_line[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		}
	}
};