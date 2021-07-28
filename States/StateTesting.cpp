#pragma once
#include "StateTesting.hpp"
#include "DrawableBuilder.hpp"
#include <iostream>

StateTesting::StateTesting()
{
	m_manager = nullptr;
	m_car = nullptr;
}

StateTesting::~StateTesting()
{
	delete m_manager;
	delete m_car;
}

void StateTesting::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		m_carFactory.front().second = true;
		m_manager->calculateFitness(m_carFactory.front(), m_fitnessVector.front());
		std::cout << m_fitnessVector.front() << std::endl;
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

void StateTesting::load()
{
	DrawableBuilder builder;
	if (builder.load())
	{
		m_manager = builder.getDrawableManager();
		m_car = builder.getDrawableCar();
		m_carFactory.push_back(std::pair(m_car, true));
		m_fitnessVector.push_back(0);
	}
}

void StateTesting::draw()
{
	for (auto& car : m_carFactory)
	{
		if (!car.second)
			continue;
		car.first->draw();
	}

	m_manager->drawFinishLine();
	m_manager->drawEdges();
	m_manager->drawCheckpoints();
}
