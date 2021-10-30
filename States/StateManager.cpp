#include "StateManager.hpp"
#include "StateMapEditor.hpp"
#include "StateArtificialNeuralNetworkEditor.hpp"
#include "StateVehicleEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"
#include "FunctionEventObserver.hpp"
#include "FunctionTimerObserver.hpp"

StateManager::StateManager()
{
	m_states[MAP_EDITOR_STATE] = new StateMapEditor();
	m_states[ARTIFICIAL_NEURAL_NETWORK_EDITOR_STATE] = new StateArtificialNeuralNetworkEditor();
	m_states[VEHICLE_EDITOR_STATE] = new StateVehicleEditor();
	m_states[TRAINING_STATE] = new StateTraining();
	m_states[TESTING_STATE] = new StateTesting();
	m_currentState = MAP_EDITOR_STATE;
	m_stateText = nullptr;
	m_stateTextObserver = nullptr;
	m_framesPerSecondText = nullptr;
	m_framesPerSecondTextObserver = nullptr;
	m_displayModeText = nullptr;
	m_displayModeTextObserver = nullptr;
	m_statesStrings[MAP_EDITOR_STATE] = "Map Editor";
	m_statesStrings[ARTIFICIAL_NEURAL_NETWORK_EDITOR_STATE] = "ANN Editor";
	m_statesStrings[VEHICLE_EDITOR_STATE] = "Vehicle Editor";
	m_statesStrings[TRAINING_STATE] = "Training";
	m_statesStrings[TESTING_STATE] = "Testing";
	m_controlKeys[sf::Keyboard::Tilde] = CHANGE_STATE;
	m_controlKeys[sf::Keyboard::BackSlash] = CHANGE_DISPLAY_MODE;
	m_pressedKeys[CHANGE_STATE] = false;
	m_pressedKeys[CHANGE_DISPLAY_MODE] = false;
}

StateManager::~StateManager()
{
	for (const auto& state : m_states)
		delete state;
	delete m_stateText;
	delete m_stateTextObserver;
	delete m_framesPerSecondText;
	delete m_framesPerSecondTextObserver;
}

bool StateManager::Load()
{
	for (const auto& state : m_states)
	{
		if (!state->Load())
			return false;
	}

	// Create texts
	m_stateText = new TripleText({ "Active state:", "", "| [~]" });
	m_framesPerSecondText = new DoubleText({ "FPS:", "" });
	m_displayModeText = new TripleText({ "Display mode:", "", "| [\\]" });

	// Create observers
	m_stateTextObserver = new FunctionEventObserver<std::string>([&] { return m_statesStrings[m_currentState]; });
	m_framesPerSecondTextObserver = new FunctionTimerObserver<size_t>([&] { return size_t(1.0 / CoreWindow::GetElapsedTime()); }, 0.3);
	m_displayModeTextObserver = new FunctionEventObserver<std::string>([&] { return CoreWindow::GetDisplayColorModeString(); });

	// Set text observers
	m_stateText->SetObserver(m_stateTextObserver);
	m_framesPerSecondText->SetObserver(m_framesPerSecondTextObserver);
	m_displayModeText->SetObserver(m_displayModeTextObserver);

	// Set text positions
	m_stateText->SetPosition({ FontContext::Component(2, true), {7, true}, {4, true}, {1, true} });
	m_framesPerSecondText->SetPosition({ FontContext::Component(0), {2, true}, {1, true} });
	m_displayModeText->SetPosition({ FontContext::Component(1, true), {7, true}, {4, true}, {1, true} });

	return true;
}

void StateManager::Capture()
{
	m_states[m_currentState]->Capture();

	if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
	{
		auto eventKey = CoreWindow::GetEvent().key.code;
		auto iterator = m_controlKeys.find(eventKey);
		if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
		{
			m_pressedKeys[iterator->second] = true;
			switch (iterator->second)
			{
				case CHANGE_STATE:
					// Change state
					++m_currentState;
					if (m_currentState >= STATE_TABLE_SIZE)
						m_currentState = MAP_EDITOR_STATE;

					// Change state string representation
					m_stateTextObserver->Notify();

					// Notify new state that it has to reload its resources
					m_states[m_currentState]->Reload();
					break;
				case CHANGE_DISPLAY_MODE:
					// Change display color mode
					CoreWindow::SwitchDisplayColorMode();
					m_displayModeTextObserver->Notify();
					break;
			}
		}
	}
	else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
	{
		auto eventKey = CoreWindow::GetEvent().key.code;
		auto iterator = m_controlKeys.find(eventKey);
		if (iterator != m_controlKeys.end())
			m_pressedKeys[iterator->second] = false;
	}
}
