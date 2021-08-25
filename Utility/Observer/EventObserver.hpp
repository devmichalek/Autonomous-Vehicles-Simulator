#pragma once
#include "ObserverIf.hpp"

class EventObserver :
	public ObserverIf
{
public:

	virtual ~EventObserver()
	{
	}

	inline bool Ready() override
	{
		bool ready = m_ready;
		m_ready = false;
		return ready;
	}

	inline void Reset() override
	{
		m_ready = false;
	}

	inline void Notify() override
	{
		m_ready = true;
	}

protected:

	EventObserver() :
		m_ready(false)
	{
	}

	bool m_ready;
};