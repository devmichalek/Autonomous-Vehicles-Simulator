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

	// Resets fields that are only valid during one iteration
	void Reset();

	// Calculates highest fitness for this iteration and highest fitness overall
	void Iterate(const SimulatedVehicles& vehicles);

	// Marks leader of current iteration (vehicle with highest fitness)
	size_t MarkLeader(const SimulatedVehicles& vehicles);

	// Checks if vehicle has made improvement, if not then vehicle is set as inactive
	void Punish(SimulatedVehicles& vehicles);

	// Updates active vehicles timers
	void UpdateTimers(SimulatedVehicles& simulatedVehicles);

	// Converts fitness to fitness ratio
	inline Fitness ToFitnessRatio(const Fitness fitness) const
	{
		return fitness / m_maxFitness * 100.0;
	}

	// Returns min possible time
	inline const double GetMinTime() const
	{
		return m_minTime;
	}

	// Returns max possible time
	inline const double GetMaxTime() const
	{
		return m_maxTime;
	}

	// Returns min fitness improvement
	inline const double GetMinFitnessImprovement() const
	{
		return m_minFitnessImprovement;
	}

	// Returns max possible fitness
	inline const Fitness GetMaxFitness() const
	{
		return m_maxFitness;
	}

	// Returns contact listener
	inline std::function<void(b2Contact*)> GetBeginContactFunction()
	{
		return m_beginContactFunction;
	}

	// Returns mean required fitness improvement
	inline Fitness GetMeanRequiredFitnessImprovement() const
	{
		return m_meanRequiredFitnessImprovement;
	}

	// Returns mean required fitness improvement in ratio
	inline Fitness GetMeanRequiredFitnessImprovementRatio() const
	{
		return ToFitnessRatio(m_meanRequiredFitnessImprovement);
	}

	// Returns highest fitness in the current iteration
	inline Fitness GetHighestFitness() const
	{
		return m_highestFitness;
	}

	// Returns highest fitness in the current iteration in ratio
	inline Fitness GetHighestFitnessRatio() const
	{
		return ToFitnessRatio(m_highestFitness);
	}

	// Returns highest fitness that has been recorded so far
	inline Fitness GetHighestFitnessOverall() const
	{
		return m_highestFitnessOverall;
	}

	// Returns highest fitness that has been recorded so far in ratio
	inline Fitness GetHighestFitnessOverallRatio() const
	{
		return ToFitnessRatio(m_highestFitnessOverall);
	}

	// Returns fitness vector
	inline const FitnessVector& GetFitnessVector() const
	{
		return m_fitnessVector;
	}

	// Returns highest fitness vector
	inline const FitnessVector& GetHighestFitnessVector() const
	{
		return m_highestFitnessVector;
	}

	// Returns highest fitness ratio vector (creates new one with deep copy)
	inline FitnessVector GetHighestFitnessRatioVector() const
	{
		FitnessVector result = m_highestFitnessVector;
		for (auto& i : result)
			i = ToFitnessRatio(i);
		return result;
	}

	// Returns mean fitness vector
	inline const FitnessVector& GetMeanFitnessVector() const
	{
		return m_meanFitnessVector;
	}

	// Returns mean fitness ratio vector (creates new one with deep copy)
	inline FitnessVector GetMeanFitnessRatioVector() const
	{
		FitnessVector result = m_meanFitnessVector;
		for (auto& i : result)
			i = ToFitnessRatio(i);
		return result;
	}

	// Returns best time in current iteration
	inline double GetBestTime() const
	{
		return m_bestTime;
	}

	// Returns best time overall
	inline double GetBestTimeOverall() const
	{
		return m_bestTimeOverall;
	}

	// Returns best time vector
	inline const std::vector<double>& GetBestTimeVector() const
	{
		return m_bestTimeVector;
	}

	// Returns best time ratio vector
	inline std::vector<double> GetBestTimeRatioVector() const
	{
		// Find max element - worst time
		auto worstTime = *std::max_element(m_bestTimeVector.begin(), m_bestTimeVector.end());
		std::vector<double> result = m_bestTimeVector;
		for (auto& i : result)
			i = i / worstTime * 100.0;
		return result;
	}

	// Returns mean time vector
	inline const std::vector<double>& GetMeanTimeVector() const
	{
		return m_meanTimeVector;
	}

	// Returns mean time ratio vector
	inline std::vector<double> GetMeanTimeRatioVector() const
	{
		// Find max element - worst mean time
		auto worstTime = *std::max_element(m_meanTimeVector.begin(), m_meanTimeVector.end());
		std::vector<double> result = m_meanTimeVector;
		for (auto& i : result)
			i = i / worstTime * 100.0;
		return result;
	}

	// Returns number of punished vehicles (non active vehicles)
	inline size_t GetNumberOfPunishedVehicles() const
	{
		return m_numberOfPunishedVehicles;
	}

	// Returns number of succeeded individuals in each iteration
	inline const std::vector<size_t>& GetNumberOfSucceededIndividualsVector() const
	{
		return m_numberOfSucceededIndividualsVector;
	}

	// Returns number of succeeded individuals in each iteration in ratio
	inline std::vector<double> GetNumberOfSucceededIndividualsRatioVector()
	{
		const double populationSize = double(m_timers.size());
		std::vector<double> result(m_numberOfSucceededIndividualsVector.size());
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = double(m_numberOfSucceededIndividualsVector[i]) / populationSize * 100.0;
		return result;
	}

private:

	const double m_minTime;
	const double m_maxTime;
	const Fitness m_maxFitness;
	const double m_minFitnessImprovement;

	std::function<void(b2Contact*)> m_beginContactFunction; // Begin contact (overlap detection) function

	Fitness m_meanRequiredFitnessImprovement;
	Fitness m_highestFitness; // Highest fitness in current iteration
	Fitness m_highestFitnessOverall; // Highest fitness so far recorded
	FitnessVector m_fitnessVector; // Holds current fitness of each individual in current iteration
	FitnessVector m_previousFitnessVector; // Holds previous fitness of each individual in current iteration
	FitnessVector m_highestFitnessVector; // Highest fitness recorded in each iteration
	FitnessVector m_meanFitnessVector; // Mean fitness recorded in each iteration

	double m_bestTime; // Best time in current iteration
	double m_bestTimeOverall; // Best time so far recorded for the highest fitness
	std::vector<StoppableTimer> m_timers; // To measure time that has passed since the beggining of iteration for specific vehicle
	std::vector<double> m_bestTimeVector; // Best times recorded in each iteration for highest fitness
	std::vector<double> m_meanTimeVector; // Mean times recorded in each iteration (average life time of vehicles)

	size_t m_numberOfPunishedVehicles;
	std::vector<size_t> m_numberOfSucceededIndividualsVector; // Number of individuals that achieved highest fitness in particular iteration
};