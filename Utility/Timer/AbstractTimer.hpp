#pragma once

class AbstractTimer
{
protected:

	double m_value;
	double m_resetValue;
	double m_timeout;
	double m_multiplier;

	explicit AbstractTimer(double resetValue, double timeout, double multiplier = 1) :
		m_value(resetValue),
		m_resetValue(resetValue),
		m_timeout(timeout),
		m_multiplier(multiplier)
	{
	}

public:

	virtual ~AbstractTimer()
	{
	}

	virtual bool Update() = 0;

	// Resets value to its initial data
	inline void Reset()
	{
		m_value = m_resetValue;
	}

	// Sets new timeout value
	inline void SetTimeout(double timeout)
	{
		m_timeout = timeout;
	}

	// Returns currently set timeout
	inline double GetTimeout() const
	{
		return m_timeout;
	}

	// Sets value to timeout value
	inline void MakeTimeout()
	{
		m_value = m_timeout;
	}

	// Returns current value
	inline double GetValue() const
	{
		return m_value;
	}

	// Returns multiplier
	inline double GetMultiplier() const
	{
		return m_multiplier;
	}
};
