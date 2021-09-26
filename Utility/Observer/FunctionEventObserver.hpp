#pragma once
#include <functional>
#include "EventObserver.hpp"

template<class Type>
class FunctionEventObserver final :
	public EventObserver
{
public:

	FunctionEventObserver(const std::function<Type()> function,
						  const std::string prefix = "",
						  const std::string postfix = "") :
		EventObserver(),
		m_function(function),
		m_prefix(prefix),
		m_postfix(postfix)
	{
	}

	~FunctionEventObserver()
	{
	}

	// Calls observed function and returns its result
	// If observed function return type is different than string return type is converted
	std::string Read() const override
	{
		if constexpr (std::is_same<Type, std::string>::value)
			return m_prefix + m_function() + m_postfix;
		else if constexpr (std::is_same<Type, bool>::value)
			return m_prefix + (m_function() ? "True" : "False") + m_postfix;
		else
			return m_prefix + std::to_string(m_function()) + m_postfix;
	}

private:

	const std::function<Type()> m_function;
	const std::string m_prefix;
	const std::string m_postfix;
};