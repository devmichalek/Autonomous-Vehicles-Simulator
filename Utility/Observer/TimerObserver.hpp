#pragma once
#include "ObserverIf.hpp"
#include "CyclicTimer.hpp"

class TimerObserver :
	public ObserverIf
{
public:

	virtual ~TimerObserver()
	{
	}

	inline bool Ready() override
	{
		return m_timer.Update();
	}

	inline void Reset() override
	{
		m_timer.Reset();
	}

	inline void Notify() override
	{
		m_timer.SetTimeout();
	}

protected:

	TimerObserver(double timeout) :
		m_timer(timeout + 1, timeout)
	{
	}

private:

	CyclicTimer m_timer;
};