#pragma once
#include "StateInterface.hpp"
#include "TripleText.hpp"
#include <array>
#include <map>
#include <string>

class EventObserver;
class TimerObserver;

class StateManager final
{
public:

	StateManager();

	~StateManager();

	// Load all states resources
	bool Load();

	// Capture active state resources
	void Capture();

	// Update active state resources
	inline void Update()
	{
		m_states[m_currentState]->Update();
		m_stateText->Update();
		m_framesPerSecondText->Update();
		m_displayModeText->Update();
	}

	// Draw active state resources and its text representation
	inline void Draw()
	{
		m_states[m_currentState]->Draw();
		m_stateText->Draw();
		m_framesPerSecondText->Draw();
		m_displayModeText->Draw();
	}

private:

	enum
	{
		MAP_EDITOR_STATE,
		ARTIFICIAL_NEURAL_NETWORK_EDITOR_STATE,
		VEHICLE_EDITOR_STATE,
		TRAINING_STATE,
		TESTING_STATE,
		STATE_TABLE_SIZE
	};

	// State container
	std::array<StateInterface*, STATE_TABLE_SIZE> m_states;

	// Current state
	size_t m_currentState;

	// Text representations of a state and their observers
	TripleText* m_stateText;
	EventObserver* m_stateTextObserver;

	// Text representation of frames per second
	DoubleText* m_framesPerSecondText;
	TimerObserver* m_framesPerSecondTextObserver;

	// Display mode settings
	TripleText* m_displayModeText;
	EventObserver* m_displayModeTextObserver;

	// String representations of states
	std::array<std::string, STATE_TABLE_SIZE> m_statesStrings;

	// Control keys
	enum
	{
		CHANGE_STATE,
		CHANGE_DISPLAY_MODE,
		CONTROLS_COUNT
	};
	std::map<const sf::Keyboard::Key, size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;
};
