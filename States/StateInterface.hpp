#pragma once

class StateInterface
{
protected:
	StateInterface()
	{
	}

public:

	StateInterface(StateInterface&) = delete;

	const StateInterface& operator=(const StateInterface&) = delete;

	virtual ~StateInterface()
	{
	}

	virtual void capture()
	{
	}

	virtual void update() = 0;

	virtual bool load() = 0;

	virtual void draw() = 0;
};
