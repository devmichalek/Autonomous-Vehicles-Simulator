#pragma once
#include "Observer.hpp"

template<class Type, class CastType = Type>
class TypeObserver :
	public Observer
{
public:

	TypeObserver(const Type& data, double timeout = 0.2, std::string prefix = "", std::string postfix = "") :
		Observer(timeout), m_data(data)
	{
		m_prefix = prefix;
		m_postfix = postfix;
	}

	~TypeObserver()
	{
	}

	std::string read()
	{
		return m_prefix + std::to_string(static_cast<const CastType>(m_data)) + m_postfix;
	}

private:

	const Type& m_data;

	std::string m_prefix;

	std::string m_postfix;
};