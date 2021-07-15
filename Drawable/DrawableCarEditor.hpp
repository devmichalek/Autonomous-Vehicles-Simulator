#pragma once

class DrawableCarEditor
{
	inline static bool m_active = false;

	static void deactivate()
	{
		m_active = false;
	}

public:
	void draw();

	static bool isActive()
	{
		return m_active;
	}
};