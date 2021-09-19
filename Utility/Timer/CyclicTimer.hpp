#pragma once
#include "TimerAbstract.hpp"
#include "CoreWindow.hpp"

class CyclicTimer final :
	public TimerAbstract
{
public:

	explicit CyclicTimer(double resetValue, double timeout, double multiplier = 1) :
		TimerAbstract(resetValue, timeout, multiplier)
	{
	}

	~CyclicTimer()
	{
	}

	bool Update() override
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