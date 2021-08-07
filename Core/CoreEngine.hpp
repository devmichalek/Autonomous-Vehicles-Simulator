#pragma once

class StateManager;

class CoreEngine final
{
public:

	explicit CoreEngine();

	~CoreEngine();

private:

	// Program execution loop
	void Loop();

	// Load program resources
	bool Load();

	StateManager* m_stateManager;
};