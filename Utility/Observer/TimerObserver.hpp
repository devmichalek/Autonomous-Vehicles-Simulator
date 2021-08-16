#pragma once
#include "ObserverIf.hpp"
#include "CycleTimer.hpp"

class TimerObserver :
	public ObserverIf
{
public:

	virtual ~TimerObserver()
	{
	}

	virtual std::string Read() = 0;

	inline bool Ready()
	{
		return m_timer.Increment();
	}

	inline void Reset()
	{
		m_timer.Reset();
	}

protected:

	TimerObserver(double timeout) :
		m_timer(timeout + 1, timeout)
	{
	}

private:

	CycleTimer m_timer;
};