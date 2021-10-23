#pragma once
#include "TimerAbstract.hpp"
#include "CoreWindow.hpp"

class StoppableTimer final :
	public TimerAbstract
{
public:

	explicit StoppableTimer(double resetValue, double timeout, double multiplier = 1) :
		TimerAbstract(resetValue, timeout, multiplier)
	{
	}

	~StoppableTimer()
	{
	}

	// Updates value, in case of timeout calculation is stopped (value does not change)
	bool Update() override
	{
		double elapsedTime = CoreWindow::GetElapsedTime() * m_multiplier;

		if (m_value + elapsedTime >= m_timeout)
			return true;

		m_value += elapsedTime;
		return false;
	}
};
