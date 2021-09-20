#pragma once
#include "TimerObserver.hpp"

template<class Type, class CastType = Type>
class TypeTimerObserver final :
	public TimerObserver
{
public:

	TypeTimerObserver(const Type& data, double timeout = 0.2, std::string prefix = "", std::string postfix = "") :
		TimerObserver(timeout), m_data(data)
	{
		m_prefix = prefix;
		m_postfix = postfix;
	}

	~TypeTimerObserver()
	{
	}

	// Returns observed value
	// Result is converted to string type if observed value is of different type
	std::string Read() override
	{
		if constexpr (std::is_same<Type, std::string>::value)
			return m_prefix + m_data + m_postfix;
		else
			return m_prefix + std::to_string(static_cast<const CastType>(m_data)) + m_postfix;
	}

private:

	const Type& m_data;
	std::string m_prefix;
	std::string m_postfix;
};