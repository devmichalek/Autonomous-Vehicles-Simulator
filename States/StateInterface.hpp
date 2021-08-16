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

	virtual void Reload() = 0;

	virtual void Capture() = 0;

	virtual void Update() = 0;

	virtual bool Load() = 0;

	virtual void Draw() = 0;
};
