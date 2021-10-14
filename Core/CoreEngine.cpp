#include "CoreEngine.hpp"
#include "CoreWindow.hpp"
#include "ActivationFunctionContext.hpp"
#include "FontContext.hpp"
#include "StateManager.hpp"
#include "CoreLogger.hpp"
#include "VehicleBuilder.hpp"
#include "MapBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include <limits>

CoreEngine::CoreEngine()
{
	if (!Load())
	{
		CoreLogger::PrintError("Loading engine dependencies failed!");
		CoreWindow::GetInstance().Close();
	}
	else
	{
		CoreLogger::PrintSuccess("Correctly loaded all engine dependencies.");
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
	CoreLogger::Initialize();
	CoreWindow::Initialize();
	ActivationFunctionContext::Initialize();
	
	if (!VehicleBuilder::Initialize())
		return false;

	if (!MapBuilder::Initialize())
		return false;

	if (!ArtificialNeuralNetworkBuilder::Initialize())
		return false;

	if (!FontContext::Initialize())
		return false;

	m_stateManager = new StateManager;

	if (!m_stateManager->Load())
		return false;

	return true;
}