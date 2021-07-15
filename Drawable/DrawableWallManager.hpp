#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "Wall.hpp"

class DrawableWallManager
{
	Line m_line;
	inline static const sf::Vector2i m_blockSize = sf::Vector2i(64, 64);
	std::vector<std::vector<SegmentVector*>> m_canvas;

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
	DrawableWallManager();
	~DrawableWallManager();

	inline bool intersect(std::vector<CarPoints>& carPointsVector)
	{
		auto windowSize = CoreWindow::getSize();
		auto windowPosition = CoreWindow::getPosition();
		size_t x = windowPosition.x / m_blockSize.x;
		size_t y = windowPosition.y / m_blockSize.y;
		size_t endX = x + (windowSize.x / m_blockSize.x);
		size_t endY = y + (windowSize.y / m_blockSize.y);

		for (; x <= endX; ++x)
		{
			for (; y <= endY; ++y)
			{
				auto segments = m_canvas[x][y];
				if (!segments)
					continue;

				for (auto & segment : *segments)
				{
					for (auto& carPoints : carPointsVector)
					{
						if (intersect(segment, carPoints))
						{

						}
					}
				}
			}
		}
	}

	// Load map from file system
	void load(const char* path);


};