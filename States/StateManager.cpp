#include "StateManager.hpp"
#include "StateMapEditor.hpp"
#include "StateANNEditor.hpp"
#include "StateVehicleEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"
#include "DrawableTripleText.hpp"

StateManager::StateManager()
{
	m_states[MAP_EDITOR_STATE] = new StateMapEditor();
	m_states[ANN_EDITOR_STATE] = new StateANNEditor();
	m_states[VEHICLE_EDITOR_STATE] = new StateVehicleEditor();
	m_states[TRAINING_STATE] = new StateTraining();
	m_states[TESTING_STATE] = new StateTesting();
	m_currentState = MAP_EDITOR_STATE;
	m_stateText = new DrawableTripleText;
	m_statesStrings[MAP_EDITOR_STATE] = "Map Editor";
	m_statesStrings[ANN_EDITOR_STATE] = "ANN Editor";
	m_statesStrings[VEHICLE_EDITOR_STATE] = "Vehicle Editor";
	m_statesStrings[TRAINING_STATE] = "Training";
	m_statesStrings[TESTING_STATE] = "Testing";
	m_controlKeys[MAP_EDITOR_STATE] = std::make_pair(sf::Keyboard::F7, false);
	m_controlKeys[ANN_EDITOR_STATE] = std::make_pair(sf::Keyboard::F8, false);
	m_controlKeys[VEHICLE_EDITOR_STATE] = std::make_pair(sf::Keyboard::F9, false);
	m_controlKeys[TRAINING_STATE] = std::make_pair(sf::Keyboard::F10, false);
	m_controlKeys[TESTING_STATE] = std::make_pair(sf::Keyboard::F11, false);
}

StateManager::~StateManager()
{
	for (const auto& state : m_states)
		delete state;
	delete m_stateText;
}

bool StateManager::Load()
{
	for (const auto& state : m_states)
	{
		if (!state->Load())
			return false;
	}

	m_stateText->SetConsistentText("Active state:");
	m_stateText->SetVariableText(m_statesStrings[m_currentState]);
	m_stateText->SetInformationText("[F7] [F8] [F9] [F10] [F11] |");
	m_stateText->SetPosition({ FontContext::Component(6, true), {3, true}, {12, true}, {1, true} });

	return true;
}

void StateManager::Capture()
{
	m_states[m_currentState]->Capture();

	if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
	{
		for (auto& controlKey : m_controlKeys)
		{
			// Check if key is already pressed, if yes continue
			if (controlKey.second.second)
				continue;

			if (CoreWindow::GetEvent().key.code == controlKey.second.first)
			{
				// Mark control key as pressed
				controlKey.second.second = true;

				// Change state
				m_currentState = controlKey.first;

				// Change state string representation
				m_stateText->SetVariableText(m_statesStrings[m_currentState]);

				// Notify new state that it has to reload its resources
				m_states[m_currentState]->Reload();
				break;
			}
		}
	}
	else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
	{
		for (auto& controlKey : m_controlKeys)
		{
			// Check if key is released, if yes then continue
			if (!controlKey.second.second)
				continue;

			if (CoreWindow::GetEvent().key.code == controlKey.second.first)
			{
				// Mark control key as released
				controlKey.second.second = false;
			}
		}
	}
}

void StateManager::Update()
{
	m_states[m_currentState]->Update();
	m_stateText->Update();
}

void StateManager::Draw()
{
	m_states[m_currentState]->Draw();
	m_stateText->Draw();
}