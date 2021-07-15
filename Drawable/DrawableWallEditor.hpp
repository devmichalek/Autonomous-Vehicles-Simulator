#pragma once
#include "DrawableWallManager.hpp"

class DrawableWallEditor
{
	inline static bool m_active = false;

	friend inline void deactivate()
	{
		m_active;
	}

	friend inline bool isActive()
	{
		return m_active;
	}

	Line m_line;
	SegmentVector m_segments;

	bool m_activeSegment;
	Segment m_segment;

public:
	DrawableWallEditor();

	~DrawableWallEditor();

	void capture();

	void draw();
};