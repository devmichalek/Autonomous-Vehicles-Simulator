#pragma once
#include "ObserverInterface.hpp"
#include "ContinuousTimer.hpp"

class TimerObserver :
	public ObserverInterface
{

protected:

	// Delayed action observer constructor
	TimerObserver(double timeout) :
		m_timer(timeout + 1, timeout)
	{
	}

public:

	virtual ~TimerObserver()
	{
	}

	// Returns true if observed entity is ready
	// Observed entity is ready when timeout takes place
	inline bool Ready() override
	{
		return m_timer.Update();
	}

	// Resets timer
	inline void Reset() override
	{
		m_timer.Reset();
	}

	// Make timeout on timer
	inline void Notify() override
	{
		m_timer.MakeTimeout();
	}

private:

	ContinuousTimer m_timer;
};