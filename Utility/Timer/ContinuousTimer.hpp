#pragma once
#include "AbstractTimer.hpp"
#include "CoreWindow.hpp"

class ContinuousTimer final :
	public AbstractTimer
{
public:

	explicit ContinuousTimer(double resetValue, double timeout, double multiplier = 1) :
		AbstractTimer(resetValue, timeout, multiplier)
	{
	}

	~ContinuousTimer()
	{
	}

	// Updates value, in case of timeout calculation is keep on running
	inline bool Update() override
	{
		m_value += CoreWindow::GetElapsedTime() * m_multiplier;
		if (m_value > m_timeout)
		{
			m_value = m_resetValue;
			return true;
		}

		return false;
	}
};
