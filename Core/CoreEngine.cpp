#include "CoreEngine.hpp"
#include "CoreWindow.hpp"
#include "StateMenu.hpp"
#include "StateEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"

CoreEngine::CoreEngine()
{
	m_states.push_back(new StateMenu());
	m_states.push_back(new StateEditor());
	m_states.push_back(new StateTraining());
	m_states.push_back(new StateTesting());
	m_states.shrink_to_fit();
}

CoreEngine::~CoreEngine()
{
	for (const auto& i : m_states)
		delete i;
}

void CoreEngine::load()
{
	for (const auto& i : m_states)
		i->load();
}

void CoreEngine::loop()
{
	CoreWindow& window = CoreWindow::getInstance();
	while (window.isOpen())
	{
		// Clear
		window.clear();

		// Catch event
		while (window.isEvent())
		{
			if (window.getEvent().type == sf::Event::Closed)
				window.close();
		}

		// Restart clock
		window.restartClock();
		
		// Draw
		m_states[StateAbstract::type()]->draw();

		// Display
		window.display();
	}
}