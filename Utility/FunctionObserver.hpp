#pragma once
#include <functional>
#include "Observer.hpp"

template<class Type>
class FunctionObserver :
	public Observer
{
public:

	FunctionObserver(std::function<Type()>& function, double timeout = 0.2, std::string prefix = "", std::string postfix = "") :
		Observer(timeout), m_function(function)
	{
		m_prefix = prefix;
		m_postfix = postfix;
	}

	~FunctionObserver()
	{
	}

	std::string read()
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