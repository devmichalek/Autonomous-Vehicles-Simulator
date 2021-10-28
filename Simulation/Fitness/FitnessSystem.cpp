#pragma once
#include "FitnessSystem.hpp"
#include "SimulatedVehicle.hpp"
#include "SimulatedCheckpoint.hpp"
#include <Box2D/b2_world_callbacks.h>

FitnessSystem::FitnessSystem(const size_t populationSize,
							 const size_t checkpointCount,
							 const double minFitnessImprovement) :
	m_minTime(1.0), // 1 second
	m_maxTime(600.0), // 600 seconds
	m_maxFitness(Fitness(checkpointCount)),
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
			if (size_t(fitness) >= size_t(m_maxFitness))
				simulatedVehicle->SetInactive();
		}
	};

	m_meanRequiredFitnessImprovement = 0.0;
	m_highestFitness = 0.0;
	m_highestFitnessOverall = 0.0;
	m_fitnessVector.resize(populationSize, 0.0);
	m_previousFitnessVector.resize(populationSize, 0.0);
	//m_highestFitnessVector.clear();
	//m_meanFitnessVector.clear();

	m_bestTime = m_maxTime;
	m_bestTimeOverall = m_maxTime;
	m_timers.resize(populationSize, StoppableTimer(m_minTime, m_maxTime));
	// m_bestTimeVector.clear();
	// m_meanTimeVector.clear();
	
	m_numberOfPunishedVehicles = 0;
	// m_numberOfSucceededIndividualsVector.clear();
}

void FitnessSystem::Reset()
{
	m_meanRequiredFitnessImprovement = 0.0;
	m_highestFitness = 0.0;
	std::fill(m_fitnessVector.begin(), m_fitnessVector.end(), 0.0);
	std::fill(m_previousFitnessVector.begin(), m_previousFitnessVector.end(), 0.0);

	m_bestTime = m_maxTime;
	std::for_each(m_timers.begin(), m_timers.end(), [&](StoppableTimer& timer) { timer.Reset(); });

	m_numberOfPunishedVehicles = 0;
}

void FitnessSystem::Iterate(const SimulatedVehicles& simulatedVehicles)
{
	Fitness totalFitness = 0.0;
	const auto numberOfSimulatedVehicles = simulatedVehicles.size();
	for (size_t i = 0; i < numberOfSimulatedVehicles; ++i)
	{
		m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();
		totalFitness += m_fitnessVector[i];
		if (m_highestFitnessOverall < m_fitnessVector[i])
		{
			m_bestTimeOverall = m_maxTime; // Reset best time overall
			m_highestFitnessOverall = m_fitnessVector[i]; // New highest fitness overall
		}
	}

	m_highestFitnessVector.push_back(m_highestFitnessOverall);
	m_meanFitnessVector.push_back(totalFitness / Fitness(numberOfSimulatedVehicles));

	double totalTime = 0.0;
	size_t numberOfSucceededIndividuals = 0;
	for (size_t i = 0; i < numberOfSimulatedVehicles; ++i)
	{
		auto time = m_timers[i].GetValue();
		if (size_t(m_highestFitnessOverall) == size_t(m_fitnessVector[i]))
		{
			++numberOfSucceededIndividuals;
			if (m_bestTimeOverall > time)
				m_bestTimeOverall = time;
		}

		totalTime += time;
		m_fitnessVector[i] *= 100000;
		m_fitnessVector[i] += (1.0 / time) * 10000;
	}

	m_bestTimeVector.push_back(m_bestTimeOverall);
	m_meanTimeVector.push_back(totalTime / double(numberOfSimulatedVehicles));
	m_numberOfSucceededIndividualsVector.push_back(numberOfSucceededIndividuals);
}

size_t FitnessSystem::MarkLeader(const SimulatedVehicles& simulatedVehicles)
{
	size_t leaderIndex = 0;
	
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		simulatedVehicles[i]->SetAsFollower();
		if (!simulatedVehicles[i]->IsActive())
		{
			m_fitnessVector[i] = 0.0;
			continue;
		}

		m_highestFitness = 0.0; // Highest fitness will be reset if there is at least one active vehicle
		m_bestTime = m_maxTime; // Best time will be reset if there is at least one active vehicle

		for (; i < simulatedVehicles.size(); ++i)
		{
			simulatedVehicles[i]->SetAsFollower();
			if (!simulatedVehicles[i]->IsActive())
			{
				m_fitnessVector[i] = 0.0;
				continue;
			}

			m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();
			if (m_highestFitness < m_fitnessVector[i])
			{
				leaderIndex = i;
				m_highestFitness = m_fitnessVector[i];
			}
		}

		break;
	}
	
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		if (!simulatedVehicles[i]->IsActive())
			continue;

		if (size_t(m_highestFitness) <= size_t(m_fitnessVector[i]))
		{
			// Find best time in this interation only for active vehicles
			auto time = m_timers[i].GetValue();
			if (m_bestTime > time)
				m_bestTime = time;
		}
	}

	simulatedVehicles[leaderIndex]->SetAsLeader();
	return leaderIndex;
}

void FitnessSystem::Punish(SimulatedVehicles& simulatedVehicles)
{
	m_meanRequiredFitnessImprovement = 0.0;
	m_numberOfPunishedVehicles = 0;
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		if (!simulatedVehicles[i]->IsActive())
		{
			m_numberOfPunishedVehicles++;
			continue;
		}
		
		m_fitnessVector[i] = simulatedVehicles[i]->GetFitness();
		auto requiredFitness = m_previousFitnessVector[i];
		requiredFitness += m_maxFitness * m_minFitnessImprovement;
		if (requiredFitness > m_fitnessVector[i])
			simulatedVehicles[i]->SetInactive();

		if (requiredFitness > m_fitnessVector[i])
			m_previousFitnessVector[i] = requiredFitness;
		else
			m_previousFitnessVector[i] = m_fitnessVector[i];

		m_meanRequiredFitnessImprovement += m_previousFitnessVector[i];
	}

	size_t numberOfNotPunishedVehicles = simulatedVehicles.size() - m_numberOfPunishedVehicles;
	m_meanRequiredFitnessImprovement = m_meanRequiredFitnessImprovement / numberOfNotPunishedVehicles;
}

void FitnessSystem::UpdateTimers(SimulatedVehicles& simulatedVehicles)
{
	for (size_t i = 0; i < simulatedVehicles.size(); ++i)
	{
		if (simulatedVehicles[i]->IsActive())
			m_timers[i].Update();
	}
}
