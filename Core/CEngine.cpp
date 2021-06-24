#include "CEngine.hpp"
#include "CWindow.hpp"
#include "SMenu.hpp"
#include "SEditor.hpp"
#include "STraining.hpp"
#include "STest.hpp"

CEngine::CEngine()
{
	using namespace State;
	m_states.push_back(new Menu());
	m_states.push_back(new Editor());
	m_states.push_back(new Training());
	m_states.push_back(new Test());
	m_states.shrink_to_fit();
}

CEngine::~CEngine()
{
	for (const auto& i : m_states)
		delete i;
}

void CEngine::load()
{
	for (const auto& i : m_states)
		i->load();
}

void CEngine::loop()
{
	CWindow& window = CWindow::getInstance();
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
		m_states[State::Abstract::type()]->draw();

		// Display
		window.display();
	}
}