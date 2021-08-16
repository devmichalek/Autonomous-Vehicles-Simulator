#pragma once
#include "CoreWindow.hpp"

class StoppableTimer
{
public:

	StoppableTimer(double resetValue, double max, double min = 0, double multiplier = 1) :
		m_value(resetValue),
		m_resetValue(resetValue),
		m_max(max),
		m_min(min),
		m_multiplier(multiplier)
	{
	}

	~StoppableTimer()
	{
	}

	bool Increment()
	{
		if (m_value < m_max)
			m_value += CoreWindow::GetElapsedTime() * m_multiplier;
		else
			return true;

		return false;
	}

	bool Decrement()
	{
		if (m_value > m_min)
			m_value -= CoreWindow::GetElapsedTime() * m_multiplier;
		else
			return true;

		return false;
	}

	void Reset()
	{
		m_value = m_resetValue;
	}

	inline double& Value()
	{
		return m_value;
	}

private:

	double m_value;
	const double m_resetValue;
	const double m_max;
	const double m_min;
	const double m_multiplier;
};