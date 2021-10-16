#pragma once
#include "SimulatedVehicle.hpp"
#include "StoppableTimer.hpp"

class b2Contact;

class FitnessSystem final
{
public:

	// Initiates internal fields, resizes vectors
	FitnessSystem(const size_t populationSize,
				  const size_t checkpointCount,
				  const double minFitnessImprovement);

	~FitnessSystem()
	{
	}

	// Resets internal fields
	void Reset();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(const SimulatedVehicles& vehicles);

	// Marks leader of current iteration (vehicle with highest fitness)
	size_t MarkLeader(const SimulatedVehicles& vehicles);

	// Checks if vehicle has made improvement, if not then vehicle is set as inactive
	void Punish(SimulatedVehicles& vehicles);

	// Updates vehicle timers
	void UpdateTimers()
	{
		for (auto& timer : m_timers)
			timer.Update();
	}

	// Returns fitness vector
	inline const FitnessVector& GetFitnessVector() const
	{
		return m_fitnessVector;
	}

	// Returns max available fitness for this system
	inline Fitness GetMaxFitness()
	{
		return m_maxFitness;
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
	inline std::function<void(b2Contact*)> GetBeginContactFunction()
	{
		return m_beginContactFunction;
	}

private:

	std::function<void(b2Contact*)> m_beginContactFunction;
	const Fitness m_maxFitness;
	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	Fitness m_highestFitness;
	Fitness m_highestFitnessOverall;
	std::vector<StoppableTimer> m_timers; // To measure time that has passed since the beggining of iteration for specific vehicle
	const double m_minFitnessImprovement;
	double m_meanRequiredFitnessImprovement;
	size_t m_numberOfPunishedVehicles;
};