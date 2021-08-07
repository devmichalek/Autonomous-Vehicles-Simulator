#pragma once
#include "StateInterface.hpp"
#include "StateMenu.hpp"
#include "StateEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"
#include <array>

class StateManager
{
public:
	StateManager()
	{
		m_states[MENU_STATE] = new StateMenu();
		m_states[EDITOR_STATE] = new StateEditor();
		m_states[TRAINING_STATE] = new StateTraining();
		m_states[TESTING_STATE] = new StateTesting();
		m_currentState = TRAINING_STATE;
	}

	~StateManager()
	{
		for (const auto& state : m_states)
			delete state;
	}

	inline bool Load()
	{
		for (const auto& state : m_states)
		{
			if (!state->load())
				return false;
		}

		return true;
	}

	inline void Capture()
	{
		m_states[m_currentState]->capture();
	}

	inline void Update()
	{
		m_states[m_currentState]->update();
	}

	inline void Draw()
	{
		m_states[m_currentState]->draw();
	}

private:
	enum
	{
		MENU_STATE,
		EDITOR_STATE,
		TRAINING_STATE,
		TESTING_STATE,
		STATE_TABLE_SIZE
	};

	// State container
	std::array<StateInterface*, STATE_TABLE_SIZE> m_states;

	// Current state
	size_t m_currentState;
};
