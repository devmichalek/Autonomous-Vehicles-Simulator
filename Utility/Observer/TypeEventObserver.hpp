#pragma once
#include "EventObserver.hpp"

template<class Type, class CastType = Type>
class TypeEventObserver final :
	public EventObserver
{
public:

	TypeEventObserver(const Type& data,
					  const std::string prefix = "",
					  const std::string postfix = "") :
		EventObserver(),
		m_data(data),
		m_prefix(prefix),
		m_postfix(postfix)
	{
	}

	~TypeEventObserver()
	{
	}

	// Returns observed value
	// Result is converted to string type if observed value is of different type
	std::string Read() const override
	{
		if constexpr (std::is_same<Type, std::string>::value)
			return m_prefix + m_data + m_postfix;
		else
			return m_prefix + std::to_string(static_cast<const CastType>(m_data)) + m_postfix;
	}

private:

	const Type& m_data;
	const std::string m_prefix;
	const std::string m_postfix;
};