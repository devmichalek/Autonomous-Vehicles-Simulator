#include "CoreEngine.hpp"
#include "CoreWindow.hpp"
#include "FontContext.hpp"
#include "StateManager.hpp"
#include "CoreConsoleLogger.hpp"
#include <limits>
#include <iostream>

CoreEngine::CoreEngine()
{
	if (!Load())
	{
		CoreConsoleLogger::PrintError("Loading engine dependencies failed");
		CoreWindow::GetInstance().Close();
		CoreConsoleLogger::PrintMessage("Press Enter to continue...");
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	else
	{
		CoreConsoleLogger::PrintSuccess("Correctly loaded all engine dependencies");
		Loop();
	}
		
}

CoreEngine::~CoreEngine()
{
	delete m_stateManager;
}

void CoreEngine::Loop()
{
	CoreWindow& window = CoreWindow::GetInstance();
	while (window.IsOpen())
	{
		// Clear
		window.Clear();

		// Catch event
		while (window.IsEvent())
		{
			if (window.GetEvent().type == sf::Event::Closed)
				window.Close();

			// Capture events
			m_stateManager->Capture();
		}

		// Restart clock
		window.RestartClock();

		// Mechanics
		m_stateManager->Update();
		
		// Draw
		m_stateManager->Draw();

		// Display
		window.Display();
	}
}

bool CoreEngine::Load()
{
	CoreWindow::Initialize();
	
	if (!FontContext::Initialize())
		return false;

	m_stateManager = new StateManager;

	if (!m_stateManager->Load())
		return false;

	return true;
}