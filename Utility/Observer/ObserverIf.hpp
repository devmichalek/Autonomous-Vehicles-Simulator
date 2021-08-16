#pragma once
#include <string>

class ObserverIf
{
public:

	virtual ~ObserverIf()
	{
	}

	virtual std::string Read() = 0;

	virtual bool Ready() = 0;

	virtual void Reset() = 0;
};