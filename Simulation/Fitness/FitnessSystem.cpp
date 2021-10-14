#pragma once
#include "FitnessSystem.hpp"
#include "SimulatedVehicle.hpp"
#include "SimulatedCheckpoint.hpp"
#include <Box2D/b2_world_callbacks.h>

FitnessSystem::FitnessSystem(const size_t populationSize,
							 const size_t checkpointCount,
							 const double minFitnessImprovement) :
	m_maxFitness(Fitness(checkpointCount) + 1.0),
	m_minFitnessImprovement(minFitnessImprovement)
{
	m_beginContactFunction = [&] (b2Contact* contact) {
		if (contact->GetFixtureA()->GetFilterData().categoryBits == SimulatedAbstract::CategoryCheckpoint &&
			contact->GetFixtureB()->GetFilterData().categoryBits == SimulatedAbstract::CategoryVehicle)
		{
			auto fitness = ((SimulatedCheckpoint*)contact->GetFixtureA()->GetUserData().pointer)->GetFitness();
			auto simulatedVehicle = ((SimulatedVehicle*)contact->GetFixtureB()->GetUserData().pointer);
			if (simulatedVehicle->IsActive())
				simulatedVehicle->SetFitness(fitness);
			if (fitness >= m_maxFitness - 1.0)
				simulatedVehicle->SetInactive();
		}
	};

	m_highestFitness = 0.0;
	m_highestFitnessOverall = 0.0;
	m_fitnessVector.resize(populationSize, 0.0);
	m_previousFitnessVector.resize(populationSize, 0.0);
	m_timers.resize(populationSize, StoppableTimer(1.0, std::numeric_limits<double>::max()));
	m_meanRequiredFitnessImprovement = 0.0;
	m_numberOfPunishedVehicles = 0;
}

void FitnessSystem::Reset()
{
	m_highestFitness = 0.0;
	std::fill(m_fitnessVector.begin(), m_fitnessVector.end(), 0.0);
	std::fill(m_previousFitnessVector.begin(), m_previousFitnessVector.end(), 0.0);
	std::for_each(m_timers.begin(), m_timers.end(), [&](StoppableTimer& timer) { timer.Reset(); });
	m_meanRequiredFitnessImprovement = 0.0;
	m_numberOfPunishedVehicles = 0;
}

void FitnessSystem::Iterate(const SimulatedVehicles& simulatedVehicles)
{
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
		m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end()); // Max: 100%
	auto value = *iterator;

	if (m_highestFitnessOverall < value)
		m_highestFitnessOverall = value;
	m_highestFitness = 0.0;

	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		m_fitnessVector[i] *= 100000;
		m_fitnessVector[i] += (1.0 / m_timers[i].GetValue()) * 10000;
	}
}

size_t FitnessSystem::MarkLeader(const SimulatedVehicles& simulatedVehicles)
{
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		simulatedVehicles[i]->SetAsFollower();
		if (!simulatedVehicles[i]->IsActive())
		{
			m_fitnessVector[i] = 0.0;
			continue;
		}

		m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();
	}

	auto iterator = std::max_element(m_fitnessVector.begin(), m_fitnessVector.end());
	m_highestFitness = *iterator;
	if (m_highestFitnessOverall < m_highestFitness)
		m_highestFitnessOverall = m_highestFitness;

	size_t index = std::distance(m_fitnessVector.begin(), iterator);
	simulatedVehicles[index]->SetAsLeader();
	return index;
}

void FitnessSystem::Punish(SimulatedVehicles& simulatedVehicles)
{
	m_meanRequiredFitnessImprovement = 0.0;
	m_numberOfPunishedVehicles = 0;
	auto maxFitness = m_maxFitness;
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		if (!simulatedVehicles[i]->IsActive())
		{
			m_numberOfPunishedVehicles++;
			continue;
		}
		
		m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();
		auto requiredFitness = m_previousFitnessVector[i];
		requiredFitness += maxFitness * m_minFitnessImprovement;
		if (requiredFitness > m_fitnessVector[i])
			simulatedVehicles[i]->SetInactive();

		if (requiredFitness > m_fitnessVector[i])
			m_previousFitnessVector[i] = requiredFitness;
		else
			m_previousFitnessVector[i] = m_fitnessVector[i];

		m_meanRequiredFitnessImprovement += m_previousFitnessVector[i];
	}

	size_t numberOfNotPunishedVehicles = simulatedVehicles.size() - m_numberOfPunishedVehicles;
	m_meanRequiredFitnessImprovement = m_meanRequiredFitnessImprovement / maxFitness / numberOfNotPunishedVehicles;
}
