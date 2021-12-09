#pragma once
#include "AbstractTimer.hpp"
#include "CoreWindow.hpp"

class PeriodicTimer final :
	public AbstractTimer
{
public:

	PeriodicTimer(const double value, const double timeout, const double multiplier) :
		AbstractTimer(1, timeout, multiplier)
	{
		m_value = value;
	}

	~PeriodicTimer()
	{
	}

	// Update value with periodic function
	inline bool Update() override
	{
		m_value += CoreWindow::GetElapsedTime() * m_multiplier * m_resetValue;
		if (m_value * m_resetValue > m_timeout)
		{
			m_value = m_timeout * m_resetValue;
			m_resetValue = -m_resetValue;
			return true;
		}

		return false;
	}

	// Returns value range - m_timeout * 2
	inline double GetValueRange() const
	{
		return m_timeout * 2;
	}
};
