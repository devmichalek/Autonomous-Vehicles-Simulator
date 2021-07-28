#pragma once
#include "CycleTimer.hpp"

class Observer
{
public:

	virtual ~Observer()
	{
	}

	virtual std::string read() = 0;

	inline bool timeout()
	{
		return m_timer.increment();
	}

protected:

	CycleTimer m_timer;

	Observer(double timeout) :
		m_timer(timeout + 1, timeout)
	{
	}
};