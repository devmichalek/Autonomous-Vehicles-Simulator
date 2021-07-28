#pragma once
#include <functional>
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"
#include "DrawableMath.hpp"
#include "DrawableFinishLine.hpp"

class DrawableManager
{
	Line m_edgeLine;
	Line m_checkpointLine;
	EdgeVector m_edges;
	EdgeVector m_innerCheckpoints;
	EdgeVector m_outerCheckpoints;
	DrawableFinishLine m_finishLine;

public:
	DrawableManager(EdgeVector&& edges,
					Edge&& finishLine,
					EdgeVector&& innerCheckpoints,
					EdgeVector&& outerCheckpoints)
	{
		m_edgeLine[0].color = sf::Color::White;
		m_edgeLine[1].color = m_edgeLine[0].color;
		m_checkpointLine[0].color = sf::Color(0, 255, 0, 64);
		m_checkpointLine[1].color = m_checkpointLine[0].color;
		m_edges = std::move(edges);
		m_finishLine.set(finishLine);
		m_innerCheckpoints = std::move(innerCheckpoints);
		m_outerCheckpoints = std::move(outerCheckpoints);
	}

	~DrawableManager()
	{
	}

	inline Fitness getMaxFitness()
	{
		return Fitness(m_innerCheckpoints.size() + m_outerCheckpoints.size()) / 2;
	}

	inline void calculateFitness(std::pair<DrawableCar*, bool>& car, Fitness& fitness)
	{
		if (Intersect(m_finishLine.m_edge, car.first->m_points))
		{
			fitness = Fitness(-1);
		}
		else
		{
			Fitness innerFitness = 0;
			for (size_t i = 0; i < m_innerCheckpoints.size(); ++i)
			{
				if (Intersect(m_innerCheckpoints[i], car.first->m_points))
				{
					innerFitness = Fitness(i - 1);
				}
			}

			Fitness outerFitness = 0;
			for (size_t i = 0; i < m_outerCheckpoints.size(); ++i)
			{
				if (Intersect(m_outerCheckpoints[i], car.first->m_points))
				{
					outerFitness = Fitness(i - 1);
				}
			}

			if (innerFitness && outerFitness)
			{
				fitness = (innerFitness + outerFitness) / 2;
			}
			else
			{
				auto max = (m_innerCheckpoints.size() + m_outerCheckpoints.size()) / 2;
				double percentage;
				if (innerFitness)
					percentage = double(innerFitness) / m_innerCheckpoints.size();
				else
					percentage = double(outerFitness) / m_outerCheckpoints.size();
				fitness = Fitness(percentage * max);
			}
		}
	}

	inline void calculateFitness(DrawableCarFactory& cars, FitnessVector& fitnessVector, std::vector<double>& timers)
	{
		auto max = getMaxFitness();
		for (size_t i = 0; i < cars.size(); ++i)
		{
			calculateFitness(cars[i], fitnessVector[i]);
			fitnessVector[i] += static_cast<Fitness>(double(max) / timers[i]);
		}
	}

	inline void intersect(DrawableCarFactory& cars)
	{
		for (auto& car : cars)
		{
			if (!car.second)
				continue;

			if (Intersect(m_finishLine.m_edge, car.first->m_points))
			{
				car.second = false;
			}

			for (auto & edge : m_edges)
			{
				if (Intersect(edge, car.first->m_points))
				{
					car.second = false;
				}
				else
					car.first->detect(edge);
			}
		}
	}

	inline void drawFinishLine()
	{
		m_finishLine.draw();
	}

	inline void drawEdges()
	{
		for (const auto& i : m_edges)
		{
			m_edgeLine[0].position = i[0];
			m_edgeLine[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_edgeLine.data(), 2, sf::Lines);
		}
	}

	inline void drawCheckpoints()
	{
		for (const auto& i : m_innerCheckpoints)
		{
			m_checkpointLine[0].position = i[0];
			m_checkpointLine[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_checkpointLine.data(), 2, sf::Lines);
		}

		for (const auto& i : m_outerCheckpoints)
		{
			m_checkpointLine[0].position = i[0];
			m_checkpointLine[1].position = i[1];
			CoreWindow::getRenderWindow().draw(m_checkpointLine.data(), 2, sf::Lines);
		}
	}
};