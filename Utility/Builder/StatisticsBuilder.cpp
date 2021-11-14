#include "StatisticsBuilder.hpp"

StatisticsBuilder::StatisticsBuilder()
{
	m_operationsMap.insert(std::pair(SUCCESS_SAVE_COMPLETED, "Success: correctly saved file!"));
	m_operationsMap.insert(std::pair(ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING, "Error: filename is empty, cannot open file for writing!"));
	m_operationsMap.insert(std::pair(ERROR_CANNOT_OPEN_FILE_FOR_WRITING, "Error: cannot open file for writing!"));
	m_lastOperationStatus = ERROR_UNKNOWN;
}

StatisticsBuilder::~StatisticsBuilder()
{
}

void StatisticsBuilder::Clear()
{
	m_footer.clear();
	m_chartData.clear();
}

void StatisticsBuilder::ExtractStatic(const GeneticAlgorithmNeuron* geneticAlgorithm,
									  const FitnessSystem* fitnessSystem,
									  const bool deathOnEdgeContact,
									  const double requiredFitnessImprovementRise)
{
	Clear();

	AddToFooter("Number of generations", geneticAlgorithm->GetNumberOfGenerations());
	AddToFooter("Chromosome length", geneticAlgorithm->GetChromosomeLength());
	AddToFooter("Population size", geneticAlgorithm->GetPopulationSize());
	AddToFooter("Crossover type", CrossoverTypeStrings[geneticAlgorithm->GetCrossoverType()]);
	AddToFooter("Repeat crossover per individual", geneticAlgorithm->IsRepeatCrossoverPerIndividual());
	AddToFooter("Mutation probability", geneticAlgorithm->GetMutationProbability());
	AddToFooter("Decrease mutation probability over generations", geneticAlgorithm->IsDecreaseMutationProbabilityOverGenerations());
	AddToFooter("Number of parents", geneticAlgorithm->GetNumberOfParents());
	AddToFooter("Precision", geneticAlgorithm->GetPrecision());
	AddToFooter("Lower bound of range", geneticAlgorithm->GetLowerBoundOfRange());
	AddToFooter("Lower upper of range", geneticAlgorithm->GetUpperBoundOfRange());
	AddToFooter("Max fitness", fitnessSystem->GetMaxFitness());
	AddToFooter("Min time", fitnessSystem->GetMinTime());
	AddToFooter("Max time", fitnessSystem->GetMaxTime());
	AddToFooter("Min required fitness improvement", fitnessSystem->GetMinFitnessImprovement());
	AddToFooter("Min required fitness improvement rise", requiredFitnessImprovementRise);
	AddToFooter("Death on edge contact", deathOnEdgeContact);
}

void StatisticsBuilder::Extract(const size_t generation,
								FitnessSystem* fitnessSystem)
{
	AddToFooter("Generation", generation);

	const auto& highestFitnessVector = fitnessSystem->GetHighestFitnessVector();
	const auto highestFitnessRatioVector = fitnessSystem->GetHighestFitnessRatioVector();
	const auto& meanFitnessVector = fitnessSystem->GetMeanFitnessVector();
	const auto meanFitnessRatioVector = fitnessSystem->GetMeanFitnessRatioVector();
	const auto& numberOfSucceededIndividualsVector = fitnessSystem->GetNumberOfSucceededIndividualsVector();
	const auto numberOfSucceededIndividualsRatioVector = fitnessSystem->GetNumberOfSucceededIndividualsRatioVector();
	const auto& bestTimeVector = fitnessSystem->GetBestTimeVector();
	const auto bestTimeRatioVector = fitnessSystem->GetBestTimeRatioVector();
	const auto& meanTimeVector = fitnessSystem->GetMeanTimeVector();
	const auto meanTimeRatioVector = fitnessSystem->GetMeanTimeRatioVector();
	const size_t length = highestFitnessVector.size();
	for (size_t i = 0; i < length; ++i)
	{
		AddChartData(highestFitnessRatioVector[i],
					 meanFitnessRatioVector[i],
					 numberOfSucceededIndividualsRatioVector[i],
					 bestTimeRatioVector[i],
					 meanTimeRatioVector[i],
					 highestFitnessVector[i],
					 meanFitnessVector[i],
					 numberOfSucceededIndividualsVector[i],
					 bestTimeVector[i],
					 meanTimeVector[i]);
	}
}

bool StatisticsBuilder::Save(std::string filename)
{
	// Check if filename is not empty
	if (filename.empty())
	{
		m_lastOperationStatus = ERROR_EMPTY_FILENAME_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Check if file can be opened for writing
	std::ofstream output(filename, std::ios::out);
	if (!output.is_open())
	{
		m_lastOperationStatus = ERROR_CANNOT_OPEN_FILE_FOR_WRITING;
		return false;
	}

	// Save chart data
	for (const auto& i : m_chartData)
		output << i << std::endl;
	m_chartData.clear(); // Clear chart data
	output << std::endl;

	// Save footer
	for (const auto& i : m_footer)
		output << i << std::endl;
	// Remove generation information
	m_footer.pop_back();

	m_lastOperationStatus = SUCCESS_SAVE_COMPLETED;
	return true;
}

std::pair<bool, std::string> StatisticsBuilder::GetLastOperationStatus()
{
	std::string message = m_operationsMap[m_lastOperationStatus];
	switch (m_lastOperationStatus)
	{
		case SUCCESS_SAVE_COMPLETED:
			return std::make_pair(true, message);
		default:
			return std::make_pair(false, message);
	}

	return std::make_pair(false, message);
}

void StatisticsBuilder::AddChartData(double highestFitnessRatioInCurrentIteration,
									 double meanFitnessRatioInCurrentIteration,
									 double numberOfSucceededIndividualsRatioInCurrentInteration,
									 double bestTimeRatioInCurrentIteration,
									 double meanTimeRatioInCurrentIteration,
									 Fitness highestFitnessInCurrentIteration,
									 Fitness meanFitnessInCurrentIteration,
									 size_t numberOfSucceededIndividualsInCurrentInteration,
									 double bestTimeInCurrentIteration,
									 double meanTimeInCurrentIteration)
{
	std::string result;
	result += std::to_string(highestFitnessRatioInCurrentIteration) + ";";
	result += std::to_string(meanFitnessRatioInCurrentIteration) + ";";
	result += std::to_string(numberOfSucceededIndividualsRatioInCurrentInteration) + ";";
	result += std::to_string(bestTimeRatioInCurrentIteration) + ";";
	result += std::to_string(meanTimeRatioInCurrentIteration) + ";";
	result += std::to_string(highestFitnessInCurrentIteration) + ";";
	result += std::to_string(meanFitnessInCurrentIteration) + ";";
	result += std::to_string(numberOfSucceededIndividualsInCurrentInteration) + ";";
	result += std::to_string(bestTimeInCurrentIteration) + ";";
	result += std::to_string(meanTimeInCurrentIteration) ;
	m_chartData.push_back(result);
}
