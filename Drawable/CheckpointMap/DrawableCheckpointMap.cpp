#pragma once
#include "DrawableCheckpointMap.hpp"
#include "DrawableCar.hpp"

DrawableCheckpointMap::DrawableCheckpointMap()
{
	m_highestFitness = 0;
	m_highestFitnessOverall = 0;
}

DrawableCheckpointMap::~DrawableCheckpointMap()
{
}

void DrawableCheckpointMap::iterate(DetailedCarFactory& factory)
{
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		m_fitnessVector[i] = calculateFitness(factory[i]);
		m_fitnessVector[i] += static_cast<Fitness>(double(maxFitness) / m_timers[i].value());
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;
}

size_t DrawableCheckpointMap::markLeader(DetailedCarFactory& factory)
{
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		factory[i].first->setFollowerColor();
		if (!factory[i].second)
		{
			m_fitnessVector[i] = 0;
			continue;
		}
		m_fitnessVector[i] = calculateFitness(factory[i]);
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;

	size_t index = std::distance(m_fitnessVector.begin(), iterator);
	factory[index].first->setLeaderColor();
	return index;
}

void DrawableCheckpointMap::punish(DetailedCarFactory& factory)
{
	// Check if car has made improvement
	// If car has made improvement then it is not punished
	auto maxFitness = getMaxFitness();
	for (size_t i = 0; i < factory.size(); ++i)
	{
		if (!factory[i].second)
			continue;
		m_fitnessVector[i] = calculateFitness(factory[i]);
		Fitness requiredFitness = m_previousFitnessVector[i];
		requiredFitness += static_cast<Fitness>(double(maxFitness) * m_minFitnessImprovement);
		if (requiredFitness > m_fitnessVector[i])
			factory[i].second = false;
		m_previousFitnessVector[i] = m_fitnessVector[i];
	}
}

void DrawableCheckpointMap::reset()
{
	for (size_t i = 0; i < m_fitnessVector.size(); ++i)
	{
		m_fitnessVector[i] = 0;
		m_previousFitnessVector[i] = 0;
		m_timers[i].minimize();
	}
}

void DrawableCheckpointMap::restart(size_t size, double minFitnessImprovement)
{
	m_fitnessVector.resize(size, 0);
	m_previousFitnessVector.resize(size, 0);
	m_timers.resize(size, StoppableTimer(0, std::numeric_limits<double>::max()));
	m_minFitnessImprovement = minFitnessImprovement;
}

void DrawableCheckpointMap::incrementTimers()
{
	for (auto& timer : m_timers)
		timer.increment();
}

const FitnessVector& DrawableCheckpointMap::getFitnessVector() const
{
	return m_fitnessVector;
}

const Fitness& DrawableCheckpointMap::getHighestFitness() const
{
	return m_highestFitness;
}

const Fitness& DrawableCheckpointMap::getHighestFitnessOverall() const
{
	return m_highestFitnessOverall;
}
