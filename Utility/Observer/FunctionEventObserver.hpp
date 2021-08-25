#pragma once
#include <functional>
#include "EventObserver.hpp"

template<class Type>
class FunctionEventObserver final :
	public EventObserver
{
public:

	FunctionEventObserver(std::function<Type()> function, std::string prefix = "", std::string postfix = "") :
		EventObserver(), m_function(function)
	{
		m_prefix = prefix;
		m_postfix = postfix;
	}

	~FunctionEventObserver()
	{
	}

	std::string Read() override
	{
		if constexpr (std::is_same<Type, std::string>::value)
			return m_prefix + m_function() + m_postfix;
		else
			return m_prefix + std::to_string(m_function()) + m_postfix;
	}

private:

	std::function<Type()> m_function;
	std::string m_prefix;
	std::string m_postfix;
};