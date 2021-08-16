#pragma once
#include "ObserverIf.hpp"

class EventObserver :
	public ObserverIf
{
public:

	virtual ~EventObserver()
	{
	}

	virtual std::string Read() = 0;

	inline bool Ready()
	{
		bool ready = m_ready;
		m_ready = false;
		return ready;
	}

	inline void Notify()
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