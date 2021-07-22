#pragma once
#include "StateTesting.hpp"
#include "DrawableBuilder.hpp"

StateTesting::StateTesting()
{
	m_wallManager = nullptr;
	m_car = nullptr;
	m_finishLine = nullptr;
}

StateTesting::~StateTesting()
{
	delete m_wallManager;
	delete m_car;
	delete m_finishLine;
}

void StateTesting::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_car->rotate(-1.0);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_car->rotate(1.0);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_car->accelerate(1.0);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_car->brake(1.0);
	}

	m_car->update();
	m_wallManager->intersect(m_carFactory);
}

void StateTesting::load()
{
	DrawableBuilder builder;
	if (builder.load())
	{
		m_wallManager = builder.getDrawableWallManager();
		m_car = builder.getDrawableCar();
		m_finishLine = builder.getFinishLine();
		m_carFactory.push_back(std::pair(m_car, true));
	}
}

void StateTesting::draw()
{
	for (auto& i : m_carFactory)
		i.first->draw();
	m_finishLine->draw();
	m_wallManager->draw();
}
