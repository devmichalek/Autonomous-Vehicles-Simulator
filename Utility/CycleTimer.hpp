#pragma once
#include "CoreWindow.hpp"

class CycleTimer
{
public:

	CycleTimer(double value, double max, double min = 0, double multiplier = 1) :
		m_value(value),
		m_max(max),
		m_min(min),
		m_multiplier(multiplier)
	{
	}

	~CycleTimer()
	{
	}

	bool increment()
	{
		m_value += CoreWindow::getElapsedTime() * m_multiplier;
		if (m_value > m_max)
		{
			m_value = m_min;
			return true;
		}

		return false;
	}

	bool decrement()
	{
		m_value -= CoreWindow::getElapsedTime() * m_multiplier;
		if (m_value < m_min)
		{
			m_value = m_max;
			return true;
		}

		return false;
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