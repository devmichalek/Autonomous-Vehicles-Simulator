#pragma once
#include "CoreWindow.hpp"

class PeriodicTimer final
{
	double m_value;
	double m_boundaryValue;
	double m_multiplier;
	double m_sign;

public:

	// Periodic timer constructor that takes boundary and multiplier
	PeriodicTimer(double boundaryValue, double multiplier = 1) :
		m_value(-boundaryValue),
		m_boundaryValue(boundaryValue),
		m_multiplier(multiplier)
	{
		m_sign = 1;
	}

	~PeriodicTimer()
	{
	}

	// Update value with periodic function
	void Update()
	{
		m_value += CoreWindow::GetElapsedTime() * m_multiplier * m_sign;
		if (m_value * m_sign > m_boundaryValue)
		{
			m_value = m_boundaryValue * m_sign;
			m_sign = -m_sign;
		}
	}

	// Sets value
	inline void SetValue(double value)
	{
		m_value = value;
	}

	// Returns current value
	// Value is in a range of previously set boundary
	inline double GetValue() const
	{
		return m_value;
	}

	// Sets minimum value
	inline void SetBoundaryValue(double boundaryValue)
	{
		m_boundaryValue = boundaryValue;
	}

	// Returns boundary value
	inline double GetBoundaryValue() const
	{
		return m_boundaryValue;
	}

	// Returns value range - m_boundaryValue * 2
	inline double GetValueRange() const
	{
		return m_boundaryValue * 2;
	}

	// Sets multiplier
	inline void SetMultiplier(double multiplier)
	{
		m_multiplier = multiplier;
	}

	// Returns multiplier
	inline double GetMultiplier() const
	{
		return m_multiplier;
	}
};
