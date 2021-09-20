#pragma once
#include "CoreWindow.hpp"

class PeriodicTimer
{
	double m_value;
	double m_minValue;
	double m_maxValue;
	double m_multiplier;
	double m_sign;

public:

	// Periodic timer constructor that takes min, max and multiplier
	PeriodicTimer(double minValue, double maxValue, double multiplier = 1) :
		m_value(minValue),
		m_minValue(minValue),
		m_maxValue(maxValue),
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

		if (m_value > m_maxValue)
		{
			m_value = m_maxValue;
			m_sign *= -1;
		}
		else if (m_value < m_minValue)
		{
			m_value = m_minValue;
			m_sign *= -1;
		}
	}

	// Sets value
	void SetValue(double value)
	{
		m_value = value;
	}

	// Returns current value
	// Value is in a range of previously set boundaries (min; max)
	double GetValue()
	{
		return m_value;
	}

	// Sets minimum value
	void SetMinValue(double minValue)
	{
		m_minValue = minValue;
	}

	// Sets maximum value
	void SetMaxValue(double maxValue)
	{
		m_maxValue = maxValue;
	}

	// Returns value range | max - min |
	double GetValueRange()
	{
		return std::fabs(m_maxValue - m_minValue);
	}

	// Sets multiplier
	void SetMultiplier(double multiplier)
	{
		m_multiplier = multiplier;
	}
};