#pragma once
#include <string>

class ObserverInterface
{
public:

	virtual ~ObserverInterface()
	{
	}

	// Reads observed value
	virtual std::string Read() const = 0;

	// Returns true if value was marked as updated
	virtual bool Ready() = 0;

	// Resets observer
	virtual void Reset() = 0;

	// Notify observer that action took place
	virtual void Notify() = 0;

	// Returns true if observer is a timer one
	virtual bool IsTimerType() const = 0;

	// Returns true if observer is a event one
	virtual bool IsEventType() const = 0;
};