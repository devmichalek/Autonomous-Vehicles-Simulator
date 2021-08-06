#include "CoreEngine.hpp"
#include "CoreWindow.hpp"
#include "StateMenu.hpp"
#include "StateEditor.hpp"
#include "StateTraining.hpp"
#include "StateTesting.hpp"
#include "FontContext.hpp"

CoreEngine::CoreEngine()
{
	CoreWindow::initialize();
	FontContext::initialize();
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

bool CoreEngine::load()
{
	for (const auto& i : m_states)
	{
		if (!i->load())
			return false;
	}

	return true;
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

			// Capture events
			m_states[StateAbstract::type()]->capture();
		}

		// Restart clock
		window.restartClock();

		// Mechanics
		m_states[StateAbstract::type()]->update();
		
		// Draw
		m_states[StateAbstract::type()]->draw();

		// Display
		window.display();
	}
}

void CoreEngine::errorLoop()
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

		// Display
		window.display();
	}
}
