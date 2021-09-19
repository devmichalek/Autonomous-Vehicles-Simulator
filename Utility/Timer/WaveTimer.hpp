#pragma once
#include "CoreWindow.hpp"

class WaveTimer
{
	double m_value;
	double m_minValue;
	double m_maxValue;
	double m_multiplier;
	double m_sign;

public:

	WaveTimer(double minValue, double maxValue, double multiplier = 1) :
		m_value(minValue),
		m_minValue(minValue),
		m_maxValue(maxValue),
		m_multiplier(multiplier)
	{
		m_sign = minValue < maxValue ? 1 : -1;
	}

	~WaveTimer()
	{
	}

	bool Update()
	{
		m_value += CoreWindow::GetElapsedTime() * m_multiplier * m_sign;

		if (m_value > m_maxValue)
		{
			m_value = m_maxValue;
			m_sign *= -1;
			return true;
		}
		
		if (m_value < m_minValue)
		{
			m_value = m_minValue;
			m_sign *= -1;
			return true;
		}

		return false;
	}

	void SetValue(double value)
	{
		m_value = value;
	}

	double Value()
	{
		return m_value;
	}

	void SetMinValue(double minValue)
	{
		m_minValue = minValue;
	}

	double Min()
	{
		return m_minValue;
	}

	void SetMaxValue(double maxValue)
	{
		m_maxValue = maxValue;
	}

	double Max()
	{
		return m_maxValue;
	}

	void SetMultiplier(double multiplier)
	{
		m_multiplier = multiplier;
	}
};