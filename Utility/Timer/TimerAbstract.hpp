#pragma once

class TimerAbstract
{
protected:

	double m_value;
	const double m_resetValue;
	double m_timeout;
	const double m_multiplier;

	explicit TimerAbstract(double resetValue, double timeout, double multiplier = 1) :
		m_value(resetValue),
		m_resetValue(resetValue),
		m_timeout(timeout),
		m_multiplier(multiplier)
	{
	}

public:

	virtual ~TimerAbstract()
	{
	}

	virtual bool Update() = 0;

	inline void Reset()
	{
		m_value = m_resetValue;
	}

	inline void ResetTimeout(double timeout)
	{
		m_timeout = timeout;
	}

	inline void SetTimeout()
	{
		m_value = m_timeout;
	}

	inline double GetTimeout()
	{
		return m_timeout;
	}

	inline double Value()
	{
		return m_value;
	}
};