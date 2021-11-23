#pragma once
#include "AbstractTimer.hpp"
#include "CoreWindow.hpp"

class StoppableTimer final :
	public AbstractTimer
{
public:

	explicit StoppableTimer(double resetValue, double timeout, double multiplier = 1) :
		AbstractTimer(resetValue, timeout, multiplier)
	{
	}

	~StoppableTimer()
	{
	}

	// Updates value, in case of timeout calculation is stopped (value does not change)
	bool Update() override
	{
		const double elapsedTime = CoreWindow::GetElapsedTime() * m_multiplier;

		if (m_value + elapsedTime >= m_timeout)
			return true;

		m_value += elapsedTime;
		return false;
	}
};
