#pragma once
#include "CoreWindow.hpp"

class CycleTimer
{
public:

	explicit CycleTimer(double resetValue, double max, double min = 0, double multiplier = 1) :
		m_value(resetValue),
		m_resetValue(resetValue),
		m_max(max),
		m_min(min),
		m_multiplier(multiplier)
	{
	}

	~CycleTimer()
	{
	}

	bool Increment()
	{
		m_value += CoreWindow::GetElapsedTime() * m_multiplier;
		if (m_value > m_max)
		{
			m_value = m_min;
			return true;
		}

		return false;
	}

	bool Decrement()
	{
		m_value -= CoreWindow::GetElapsedTime() * m_multiplier;
		if (m_value < m_min)
		{
			m_value = m_max;
			return true;
		}

		return false;
	}

	inline double& Value()
	{
		return m_value;
	}

	inline void Reset()
	{
		m_value = m_resetValue;
	}

private:

	double m_value;
	const double m_resetValue;
	const double m_max;
	const double m_min;
	const double m_multiplier;
};