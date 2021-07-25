#pragma once
#include "StateTesting.hpp"
#include "DrawableBuilder.hpp"

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
	}
}

void StateTesting::draw()
{
	for (auto& i : m_carFactory)
	{
		if (!i.second)
			continue;
		i.first->draw();
	}

	m_manager->draw();
}
