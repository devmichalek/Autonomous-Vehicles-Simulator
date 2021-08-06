#pragma once
#include "StateTesting.hpp"
#include "DrawableBuilder.hpp"
#include <iostream>

StateTesting::StateTesting()
{
	m_manager = nullptr;
	m_car = nullptr;
	m_checkpointMap = nullptr;
}

StateTesting::~StateTesting()
{
	delete m_manager;
	delete m_car;
	delete m_checkpointMap;
}

void StateTesting::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		m_carFactory.front().second = true;
		m_checkpointMap->iterate(m_carFactory, m_manager->getFinishLine());
		std::cout << m_checkpointMap->getHighestFitness() << std::endl;
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
	m_manager->intersect(m_carFactory);

	auto& view = CoreWindow::getView();
	view.setCenter(m_car->getCenter());
	CoreWindow::getRenderWindow().setView(view);
}

bool StateTesting::load()
{
	DrawableBuilder builder;
	if (!builder.Load())
	{
		return false;
	}

	m_checkpointMap = builder.GetDrawableCheckpointMap();
	m_manager = builder.GetDrawableManager();
	m_car = builder.GetDrawableCar();
	m_carFactory.push_back(std::pair(m_car, true));
	m_checkpointMap->restart(m_carFactory.size(), 0.02);

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

	m_manager->drawFinishLine();
	m_manager->drawEdges();
	m_checkpointMap->draw();
}
