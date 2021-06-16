#include "Engine.hpp"
#include "Window.hpp"

Engine::Engine()
{
	
}

Engine::~Engine()
{
	
}

void Engine::load()
{
	
}

void Engine::loop()
{
	Window& window = Window::getInstance();
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

		// Display
		window.display();
	}
}