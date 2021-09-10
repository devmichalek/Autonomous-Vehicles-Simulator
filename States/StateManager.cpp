#include "StateManager.hpp"
#include "StateMapEditor.hpp"
#include "StateANNEditor.hpp"
#include "StateVehicleEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"
#include "DrawableTripleText.hpp"
#include "FunctionEventObserver.hpp"

StateManager::StateManager()
{
	m_states[MAP_EDITOR_STATE] = new StateMapEditor();
	m_states[ANN_EDITOR_STATE] = new StateANNEditor();
	m_states[VEHICLE_EDITOR_STATE] = new StateVehicleEditor();
	m_states[TRAINING_STATE] = new StateTraining();
	m_states[TESTING_STATE] = new StateTesting();
	m_currentState = MAP_EDITOR_STATE;
	m_stateText = nullptr;
	m_stateTextObserver = nullptr;
	m_statesStrings[MAP_EDITOR_STATE] = "Map Editor";
	m_statesStrings[ANN_EDITOR_STATE] = "ANN Editor";
	m_statesStrings[VEHICLE_EDITOR_STATE] = "Vehicle Editor";
	m_statesStrings[TRAINING_STATE] = "Training";
	m_statesStrings[TESTING_STATE] = "Testing";
	m_controlKey = std::make_pair(sf::Keyboard::Tilde, false);
}

StateManager::~StateManager()
{
	for (const auto& state : m_states)
		delete state;
	delete m_stateText;
	delete m_stateTextObserver;
}

bool StateManager::Load()
{
	for (const auto& state : m_states)
	{
		if (!state->Load())
			return false;
	}

	m_stateText = new DrawableTripleText({ "Active state:", "", "| [~]" });
	m_stateText->SetPosition({ FontContext::Component(1, true), {7, true}, {4, true}, {1, true} });
	m_stateTextObserver = new FunctionEventObserver<std::string>([&] { return m_statesStrings[m_currentState]; });
	m_stateText->SetObserver(m_stateTextObserver);

	return true;
}

void StateManager::Capture()
{
	m_states[m_currentState]->Capture();

	if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
	{
		// Check if key is already pressed, if no then continue
		if (!m_controlKey.second && CoreWindow::GetEvent().key.code == m_controlKey.first)
		{
			// Mark control key as pressed
			m_controlKey.second = true;

			// Change state
			++m_currentState;
			if (m_currentState >= STATE_TABLE_SIZE)
				m_currentState = MAP_EDITOR_STATE;

			// Change state string representation
			m_stateTextObserver->Notify();

			// Notify new state that it has to reload its resources
			m_states[m_currentState]->Reload();
		}
	}
	else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
	{
		// Check if key is released, if no then continue
		if (m_controlKey.second && CoreWindow::GetEvent().key.code == m_controlKey.first)
		{
			// Mark control key as released
			m_controlKey.second = false;
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