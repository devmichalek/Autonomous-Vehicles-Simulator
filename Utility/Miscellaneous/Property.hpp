#pragma once

template<class Type>
class Property
{
public:

	explicit Property(const Type minimumValue,
					  const Type maximumValue,
					  const Type offsetValue,
					  const Type resetValue) :
		m_minimumValue(minimumValue),
		m_maximumValue(maximumValue),
		m_offsetValue(offsetValue),
		m_resetValue(resetValue),
		m_value(resetValue)
	{
	}

	operator Type() const
	{
		return m_value;
	}

	inline void SetValue(Type value)
	{
		m_value = value;
	}

	inline void ResetValue()
	{
		m_value = m_resetValue;
	}

	inline void Increase()
	{
		m_value += m_offsetValue;
		if (m_value > m_maximumValue)
			m_value = m_maximumValue;
	}

	inline void Decrease()
	{
		m_value -= m_offsetValue;
		if (m_value < m_minimumValue)
			m_value = m_minimumValue;
	}

private:

	const Type m_minimumValue;
	const Type m_maximumValue;
	const Type m_offsetValue;
	const Type m_resetValue;
	Type m_value;
};