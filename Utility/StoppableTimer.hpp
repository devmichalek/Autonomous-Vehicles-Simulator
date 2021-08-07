#pragma once
#include "CoreWindow.hpp"

class StoppableTimer
{
public:

	StoppableTimer(double value, double max, double min = 0, double multiplier = 1) :
		m_value(value),
		m_max(max),
		m_min(min),
		m_multiplier(multiplier)
	{
	}

	~StoppableTimer()
	{
	}

	bool increment()
	{
		if (m_value < m_max)
			m_value += CoreWindow::GetElapsedTime() * m_multiplier;
		else
			return true;

		return false;
	}

	bool decrement()
	{
		if (m_value > m_min)
			m_value -= CoreWindow::GetElapsedTime() * m_multiplier;
		else
			return true;

		return false;
	}

	void maximize()
	{
		m_value = m_max;
	}

	void minimize()
	{
		m_value = m_min;
	}

	inline double& value()
	{
		return m_value;
	}

private:

	const double m_max;
	const double m_min;
	const double m_multiplier;
	double m_value;
};