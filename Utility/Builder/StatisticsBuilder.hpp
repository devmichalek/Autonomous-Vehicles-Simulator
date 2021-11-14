#pragma once
#include "GeneticAlgorithm.hpp"
#include "FitnessSystem.hpp"

class StatisticsBuilder
{
public:

	StatisticsBuilder();

	~StatisticsBuilder();

	// Clears internal fields
	void Clear();

	// Extracts static statistics
	void ExtractStatic(const GeneticAlgorithmNeuron* geneticAlgorithm,
					   const FitnessSystem* fitnessSystem,
					   const bool deathOnEdgeContact,
					   const double requiredFitnessImprovementRise);

	// Extracts dynamic statistics
	void Extract(const size_t currentGeneration,
				 FitnessSystem* fitnessSystem);

	// Saves gathered data to file
	bool Save(std::string filename);

	// Returns the result of last operation, true in case of success
	std::pair<bool, std::string> GetLastOperationStatus();

private:

	// Adds string to footer with csv format
	template<class Type>
	void AddToFooter(std::string info, Type value)
	{
		if constexpr (std::is_same<Type, std::string>::value)
			info = (info + ": " + value + ";");
		else if constexpr (std::is_same<Type, const char*>::value)
			info = (info + ": " + std::string(value) + ";");
		else if constexpr (std::is_same<Type, bool>::value)
			info = (info + ": " + (value ? "True" : "False") + ";");
		else
			info = (info + ": " + std::to_string(value) + ";");
		m_footer.push_back(info);
	}

	// Adds chart data with csv format
	void AddChartData(double highestFitnessRatioInCurrentIteration,
					  double meanFitnessRatioInCurrentIteration,
					  double numberOfSucceededIndividualsRatioInCurrentInteration,
					  double bestTimeRatioInCurrentIteration,
					  double meanTimeRatioInCurrentIteration,
					  Fitness highestFitnessInCurrentIteration,
					  Fitness meanFitnessInCurrentIteration,
					  size_t numberOfSucceededIndividualsInCurrentInteration,
					  double bestTimeInCurrentIteration,
					  double meanTimeInCurrentIteration);

	std::vector<std::string> m_footer;
	std::vector<std::string> m_chartData;

	// Status
	enum
	{
		ERROR_UNKNOWN,
		SUCCESS_SAVE_COMPLETED,
		ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING,
		ERROR_CANNOT_OPEN_FILE_FOR_WRITING,
		LAST_ENUM_OPERATION_INDEX
	};
	size_t m_lastOperationStatus;
	std::map<const size_t, std::string> m_operationsMap;
};
