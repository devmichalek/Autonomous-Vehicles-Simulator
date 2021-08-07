#pragma once
#include "StateTesting.hpp"
#include "DrawableBuilder.hpp"
#include "CoreConsoleLogger.hpp"

StateTesting::StateTesting()
{
	m_edgeManager = nullptr;
	m_car = nullptr;
	m_checkpointMap = nullptr;
}

StateTesting::~StateTesting()
{
	delete m_edgeManager;
	delete m_car;
	delete m_checkpointMap;
}

void StateTesting::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		m_carFactory.front().second = true;
		m_checkpointMap->iterate(m_carFactory);
		CoreConsoleLogger::PrintMessage("Highest fitness: " + std::to_string(m_checkpointMap->getHighestFitness()));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_car->rotate(0.0);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_car->rotate(1.0);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_car->accelerate(1.0);
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_car->brake(1.0);
	}

	m_car->update();
	m_edgeManager->intersect(m_carFactory);

	auto& view = CoreWindow::GetView();
	view.setCenter(m_car->getCenter());
	CoreWindow::GetRenderWindow().setView(view);
}

bool StateTesting::load()
{
	DrawableBuilder builder;
	if (!builder.Load())
	{
		return false;
	}

	m_checkpointMap = builder.GetDrawableCheckpointMap();
	m_edgeManager = builder.GetDrawableManager();
	m_car = builder.GetDrawableCar();
	m_car->init(CAR_TWELVE_NUMBER_OF_SENSORS);
	m_carFactory.push_back(std::pair(m_car, true));
	m_checkpointMap->restart(m_carFactory.size(), 0.02);

	CoreConsoleLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::draw()
{
	for (auto& car : m_carFactory)
	{
		if (!car.second)
			continue;
		car.first->drawBody();
		car.first->drawBeams();
	}

	m_edgeManager->drawEdges();
	m_checkpointMap->draw();
}
