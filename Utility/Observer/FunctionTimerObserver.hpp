#pragma once
#include "TimerObserver.hpp"
#include <functional>

template<class Type>
class FunctionTimerObserver :
	public TimerObserver
{
public:

	FunctionTimerObserver(std::function<Type()>& function, double timeout = 0.2, std::string prefix = "", std::string postfix = "") :
		TimerObserver(timeout), m_function(function)
	{
		m_prefix = prefix;
		m_postfix = postfix;
	}

	~FunctionTimerObserver()
	{
	}

	std::string Read()
	{
		if constexpr (std::is_same<Type, std::string>::value)
			return m_prefix + m_function() + m_postfix;
		else
			return m_prefix + std::to_string(m_function()) + m_postfix;
	}

private:

	std::function<Type()>& m_function;
	std::string m_prefix;
	std::string m_postfix;
};