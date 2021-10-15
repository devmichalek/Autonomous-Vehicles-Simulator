#pragma once
#include "ObserverInterface.hpp"

class EventObserver :
	public ObserverInterface
{
public:

	virtual ~EventObserver()
	{
	}

	// Returns true if observed entity is ready
	// Observed entity is ready when someone notifies observer
	inline bool Ready() override
	{
		bool ready = m_ready;
		m_ready = false;
		return ready;
	}

	// Resets observer ready field
	inline void Reset() override
	{
		m_ready = false;
	}

	// Sets observer ready field
	inline void Notify() override
	{
		m_ready = true;
	}

	// Returns false since it is a event observer
	inline bool IsTimerType()
	{
		return false;
	}

	// Returns true since it is a event observer
	bool IsEventType()
	{
		return true;
	}

protected:

	EventObserver() :
		m_ready(true)
	{
	}

	bool m_ready;
};