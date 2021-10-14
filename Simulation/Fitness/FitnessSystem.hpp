#pragma once
#include "SimulatedVehicle.hpp"
#include "StoppableTimer.hpp"
#include <Box2D/b2_world_callbacks.h>

class FitnessSystem final
{
public:

	// Initiates internal fields, resizes vectors
	FitnessSystem(const size_t populationSize,
				  const size_t checkpointCount,
				  double minFitnessImprovement);

	~FitnessSystem()
	{
	}

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(const SimulatedVehicles& vehicles);

	// Marks leader of current iteration (vehicle with highest fitness)
	size_t MarkLeader(const SimulatedVehicles& vehicles);

	// Checks if vehicle has made improvement, if not then vehicle is set as inactive
	void Punish(SimulatedVehicles& vehicles);

	// Returns fitness vector
	inline const FitnessVector& GetFitnessVector() const
	{
		return m_fitnessVector;
	}

	// Returns highest fitness in the current iteration in ratio
	inline Fitness GetHighestFitness() const
	{
		return m_highestFitness / m_maxFitness;
	}

	// Returns highest fitness that has been recorded so far in ratio
	inline Fitness GetHighestFitnessOverall() const
	{
		return m_highestFitnessOverall / m_maxFitness;
	}

	// Returns mean required fitness improvement ratio
	inline double GetMeanRequiredFitnessImprovement() const
	{
		return m_meanRequiredFitnessImprovement;
	}

	// Returns number of punished vehicles (non active vehicles)
	inline size_t GetNumberOfPunishedVehicles() const
	{
		return m_numberOfPunishedVehicles;
	}

	// Returns contact listener
	inline b2ContactListener* GetContactListener()
	{
		return &m_fitnessListener;
	}

private:

	class FitnessListener :
		public b2ContactListener
	{
		Fitness m_fitnessCeiling;

	public:
		FitnessListener(Fitness fitnessCeiling) :
			m_fitnessCeiling(fitnessCeiling)
		{
		}

		// Detects begin contact
		void BeginContact(b2Contact* contact);
	};

	FitnessListener m_fitnessListener;
	const Fitness m_maxFitness;
	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers; // To measure time that has passed since the beggining of iteration for specific vehicle
	double m_minFitnessImprovement;
	double m_meanRequiredFitnessImprovement;
	size_t m_numberOfPunishedVehicles;
};