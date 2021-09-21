#pragma once

class StateInterface
{
protected:

	StateInterface()
	{
	}

	StateInterface(StateInterface&) = delete;

	const StateInterface& operator=(const StateInterface&) = delete;

	virtual ~StateInterface()
	{
	}

	// Reloads state internal fields
	// This function should be called when current state changes
	virtual void Reload() = 0;

	// Handles captured events
	virtual void Capture() = 0;

	// Updates state mechanics
	// This function should be called in every frame
	virtual void Update() = 0;

	// Loads state internal fields
	virtual bool Load() = 0;

	// Draws state internal drawable objects
	// Function should be called in every frame
	virtual void Draw() = 0;

	// Friend classes
	friend class StateManager;
};
