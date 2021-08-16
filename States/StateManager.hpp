#pragma once
#include <array>
#include <map>
#include <string>

class StateInterface;
class DrawableTripleText;

class StateManager
{
public:

	StateManager();

	~StateManager();

	// Load all states resources
	bool Load();

	// Capture active state resources
	void Capture();

	// Update active state resources
	void Update();

	// Draw active state resources and its text representation
	void Draw();

private:

	enum
	{
		MAP_EDITOR_STATE,
		ANN_EDITOR_STATE,
		VEHICLE_EDITOR_STATE,
		TRAINING_STATE,
		TESTING_STATE,
		STATE_TABLE_SIZE
	};

	// State container
	std::array<StateInterface*, STATE_TABLE_SIZE> m_states;

	// Current state
	size_t m_currentState;

	// Texts representation of a state
	DrawableTripleText* m_stateText;

	// String representations of states
	std::array<std::string, STATE_TABLE_SIZE> m_statesStrings;

	// Control keys allowing to change the state
	std::map<size_t, std::pair<size_t, bool>> m_controlKeys;
};
