#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>
#include "GeneticAlgorithm.hpp"

namespace TestGeneticAlgorithm
{
	class TestTimer
	{
		bool m_running;
		std::chrono::steady_clock::time_point m_start;

		void Finish()
		{
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - m_start;
			std::cout << "\tElapsed time: " << elapsed.count() << "s\n";
		}

	public:

		TestTimer()
		{
			m_running = true;
			m_start = std::chrono::high_resolution_clock::now();
		}

		void Stop()
		{
			if (m_running)
			{
				m_running = false;
				Finish();
			}
		}

		~TestTimer()
		{
			Stop();
		}
	};

	template <class Type>
	std::ostream& operator<<(std::ostream& os, const std::vector<Type>& rhs)
	{
		if constexpr (std::is_same<Type, char>::value)
		{
			auto rhsSize = rhs.size();
			std::string rhsResult;
			rhsResult.resize(rhsSize);
			for (auto i = 0; i < rhsSize; ++i)
				rhsResult[i] = rhs[i];
			os << rhsResult;
		}
		else if constexpr (std::is_same<Type, float>::value)
		{
			auto rhsSize = rhs.size();
			os << std::fixed << std::setprecision(2) << rhs[0];
			for (auto i = 1; i < rhsSize; ++i)
				os << ", " << std::fixed << std::setprecision(2) << rhs[i];
		}
		else if constexpr (std::is_same<Type, double>::value)
		{
			auto rhsSize = rhs.size();
			os << std::fixed << std::setprecision(4) << rhs[0];
			for (auto i = 1; i < rhsSize; ++i)
				os << ", " << std::fixed << std::setprecision(4) << rhs[i];
		}
		else
		{
			os << "There is no available chromosome representation.";
		}

		return os;
	}

	template<class Type>
	void PrintTestStatistics(const size_t numOfGenerations,
							 const double expectedResults,
							 const double results,
							 std::vector<Type>& expectedChromosome,
							 std::vector<Type>& bestChromosome)
	{
		const unsigned perfectResults = 100;
		auto percentBestResult = unsigned(results * perfectResults);
		auto percentExpectedResults = unsigned(expectedResults * perfectResults);

		std::cout << "\tExpected: " << expectedChromosome << std::endl;
		std::cout << "\tResult:   " << bestChromosome << std::endl;

		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (percentBestResult > percentExpectedResults || percentBestResult == perfectResults)
			SetConsoleTextAttribute(handle, 10);
		else if (percentBestResult == percentExpectedResults)
			SetConsoleTextAttribute(handle, 14);
		else
			SetConsoleTextAttribute(handle, 12);

		std::cout << "\tExpected to have " << expectedResults * perfectResults << "% of accuracy, ";
		std::cout << "after " << numOfGenerations << " generations received " << results * 100 << "%\n\n";
		SetConsoleTextAttribute(handle, 7);
	}

	void TestCharacters(const size_t chromosomeLength,
						const size_t populationSize,
						const size_t numOfGenerations,
						size_t crossoverType,
						bool repeatCrossover,
						double mutationProbability,
						std::string alphabet,
						bool decreaseMutationOverGenerations,
						const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		std::cout << "\t\tCrossover type: " << crossoverType << std::endl;
		std::cout << "\t\tRepeat crossover: " << repeatCrossover << std::endl;
		std::cout << "\t\tMutation probability: " << mutationProbability << std::endl;
		std::cout << "\t\tDecrease mutation over generations: " << decreaseMutationOverGenerations << std::endl;

		// Set up fitness vector
		FitnessVector fitnessVector(populationSize);

		// Initialize
		GeneticAlgorithmCharacter geneticAlgorithm(numOfGenerations, chromosomeLength, populationSize, crossoverType, repeatCrossover, mutationProbability, decreaseMutationOverGenerations, alphabet);
		const size_t numOfCombinations = alphabet.size();

		// Set up goal, all genes set to 'X' character
		std::vector<char> expectedChromosome(chromosomeLength);
		for (size_t i = 0; i < chromosomeLength; ++i)
			expectedChromosome[i] = 'X';

		// Run environment
		TestTimer testTimer;
		while (geneticAlgorithm.Iterate(fitnessVector))
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				fitnessVector[j] = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					size_t difference = std::abs(geneticAlgorithm.GetIndividualChromosome(j)[k] - expectedChromosome[k]);
					difference = size_t(std::pow(difference, 2));
					long long value = long long(numOfCombinations) - difference;
					fitnessVector[j] += Fitness(value < 0 ? 0.0 : value);
				}
			}
		}
		testTimer.Stop();

		size_t bestResultsIndex = std::distance(fitnessVector.begin(), std::max_element(fitnessVector.begin(), fitnessVector.end()));
		double bestResults = double(fitnessVector[bestResultsIndex]) / chromosomeLength / numOfCombinations;
		auto bestChromosome = geneticAlgorithm.GetIndividualChromosome(bestResultsIndex);
		PrintTestStatistics<char>(numOfGenerations, expectedResults, bestResults, expectedChromosome, bestChromosome);
	}

	void TestFloatingPoints(const size_t chromosomeLength,
							const size_t populationSize,
							const size_t numOfGenerations,
							size_t crossoverType,
							bool repeatCrossover,
							double mutationProbability,
							unsigned precision,
							std::pair<float, float> range,
							bool decreaseMutationOverGenerations,
							const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		std::cout << "\t\tCrossover type: " << crossoverType << std::endl;
		std::cout << "\t\tRepeat crossover: " << repeatCrossover << std::endl;
		std::cout << "\t\tMutation probability: " << mutationProbability << std::endl;
		std::cout << "\t\tDecrease mutation over generations: " << decreaseMutationOverGenerations << std::endl;

		// Set up fitness vector
		FitnessVector fitnessVector(populationSize);

		// Initialize
		GeneticAlgorithmFloat geneticAlgorithm(numOfGenerations, chromosomeLength, populationSize, crossoverType, repeatCrossover, mutationProbability, decreaseMutationOverGenerations, precision, range);

		// Set up goal, all genes set to 1.0
		float delta = std::fabs(range.second - range.first);
		float goalGeneData = range.second;
		float goalGeneOffset = delta / chromosomeLength;
		std::vector<float> expectedChromosome(chromosomeLength);
		for (size_t i = 0; i < chromosomeLength; ++i)
		{
			goalGeneData -= goalGeneOffset;
			expectedChromosome[i] = goalGeneData;
		}

		// Run environment
		TestTimer testTimer;
		while (geneticAlgorithm.Iterate(fitnessVector))
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				fitnessVector[j] = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					float difference = std::abs(geneticAlgorithm.GetIndividualChromosome(j)[k] - expectedChromosome[k]);
					long long longDifference = long long(difference * precision);
					longDifference = long long(std::pow(longDifference, 1.15));
					long long value = long long(delta * precision) - longDifference;
					fitnessVector[j] += Fitness(value < 0 ? 0.0 : value);
				}
			}
		}
		testTimer.Stop();

		size_t bestResultsIndex = std::distance(fitnessVector.begin(), std::max_element(fitnessVector.begin(), fitnessVector.end()));
		double bestResults = double(fitnessVector[bestResultsIndex]) / chromosomeLength / (delta * precision);
		auto bestChromosome = geneticAlgorithm.GetIndividualChromosome(bestResultsIndex);
		PrintTestStatistics<float>(numOfGenerations, expectedResults, bestResults, expectedChromosome, bestChromosome);
	}

	void TestNeurons(const size_t chromosomeLength,
					 const size_t populationSize,
					 const size_t numOfGenerations,
					 size_t crossoverType,
					 bool repeatCrossover,
					 double mutationProbability,
					 unsigned precision,
					 std::pair<Neuron, Neuron> range,
					 bool decreaseMutationOverGenerations,
					 bool singlePointCrossover,
					 const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		std::cout << "\t\tCrossover type: " << crossoverType << std::endl;
		std::cout << "\t\tRepeat crossover: " << repeatCrossover << std::endl;
		std::cout << "\t\tMutation probability: " << mutationProbability << std::endl;
		std::cout << "\t\tDecrease mutation over generations: " << decreaseMutationOverGenerations << std::endl;
		std::cout << "\t\tSingle point crossover: " << singlePointCrossover << std::endl;

		// Set up fitness vector
		FitnessVector fitnessVector(populationSize);

		// Initialize
		GeneticAlgorithmNeuron geneticAlgorithm(numOfGenerations,
												chromosomeLength,
												populationSize,
												crossoverType,
												repeatCrossover,
												mutationProbability,
												decreaseMutationOverGenerations,
												2,
												precision,
												range);

		// Set up goal, all genes set to 1.0
		Neuron delta = std::fabs(range.second - range.first);
		Neuron goalGeneData = range.second;
		Neuron goalGeneOffset = delta / chromosomeLength;
		std::vector<Neuron> expectedChromosome(chromosomeLength);
		for (size_t i = 0; i < chromosomeLength; ++i)
		{
			goalGeneData -= goalGeneOffset;
			expectedChromosome[i] = goalGeneData;
		}

		// Run environment
		TestTimer testTimer;
		while (geneticAlgorithm.Iterate(fitnessVector))
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				fitnessVector[j] = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					auto data = geneticAlgorithm.GetIndividualChromosome(j)[k];
					Neuron difference = std::fabs(data - expectedChromosome[k]);
					long long longDifference = long long(difference * precision);
					long long value = long long(delta * precision) - longDifference;
					fitnessVector[j] += Fitness(value < 0 ? 0.0 : value);
				}
			}
		}
		testTimer.Stop();

		size_t bestResultsIndex = std::distance(fitnessVector.begin(), std::max_element(fitnessVector.begin(), fitnessVector.end()));
		double bestResults = (double(fitnessVector[bestResultsIndex]) / chromosomeLength) / (delta * precision);
		auto bestChromosome = geneticAlgorithm.GetIndividualChromosome(bestResultsIndex);
		PrintTestStatistics<Neuron>(numOfGenerations, expectedResults, bestResults, expectedChromosome, bestChromosome);
	}

	void RunTests()
	{
		std::cout << "Test title: TestGeneticAlgorithm\n";

		const bool runTestGroupCharacters = true;
		const bool runTestGroupFloatingPoints = true;
		const bool runTestGroupNeurons = true;
		const bool runTestGroupIntegers = true;
		
		if (runTestGroupCharacters)
		{
			std::cout << "Test group name: TestCharacters\n";
			const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}";
			TestCharacters(20, 128, 30, 0, true, 0.1, alphabet, false, 0.9);
			TestCharacters(32, 128, 60, 0, true, 0.08, alphabet, false, 0.9);
			TestCharacters(128, 256, 90, 0, true, 0.05, alphabet, false, 0.8);
			TestCharacters(256, 512, 512, 0, true, 0.03, alphabet, true, 0.94);
			TestCharacters(192, 512, 210, 0, true, 0.03, alphabet, false, 0.92);
			TestCharacters(256, 1024, 512, 0, true, 0.03, alphabet, false, 0.9);
		}

		if (runTestGroupFloatingPoints)
		{
			std::cout << "Test group name: TestFloatingPoints\n";
			TestFloatingPoints(1, 16, 32, 0, true, 0.5, 100, std::pair(-1.0f, 1.0f), false, 0.99);
			TestFloatingPoints(4, 64, 64, 0, true, 0.4, 100, std::pair(-1.0f, 1.0f), true, 0.96);
			TestFloatingPoints(8, 64, 64, 0, true, 0.8, 1000, std::pair(-10.0f, 10.0f), true, 0.98);
			TestFloatingPoints(32, 64, 128, 0, true, 0.05, 100, std::pair(-1.0f, 1.0f), false, 0.99);
			TestFloatingPoints(64, 64, 128, 0, true, 0.04, 100, std::pair(-1.0f, 1.0f), false, 0.95);
			TestFloatingPoints(128, 128, 256, 0, true, 0.06, 100, std::pair(-1.0f, 1.0f), false, 0.92);
			TestFloatingPoints(256, 128, 256, 0, true, 0.06, 100, std::pair(-5.0f, 5.0f), false, 0.8);
		}

		if (runTestGroupNeurons)
		{
			std::cout << "Test group name: TestNeurons\n";
			TestNeurons(1, 16, 32, 0, true, 0.05, 1000, std::pair(-1.0, 1.0), false, false, 0.97);
			TestNeurons(2, 16, 32, 0, true, 0.05, 1000, std::pair(-1.0, 1.0), false, false, 0.97);
			TestNeurons(4, 32, 32, 0, true, 0.05, 1000, std::pair(-1.0, 1.0), false, false, 0.97);
			TestNeurons(8, 32, 64, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.97);
			TestNeurons(16, 64, 64, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.97);
			TestNeurons(32, 64, 64, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.97);
			TestNeurons(64, 128, 128, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.97);
			TestNeurons(128, 128, 128, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.93);
			TestNeurons(256, 128, 128, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, true, 0.85);
			TestNeurons(512, 256, 256, 0, true, 0.06, 1000, std::pair(-1.0, 1.0), false, false, 0.85); // 1.5min
			TestNeurons(1024, 256, 512, 0, true, 0.06, 1000, std::pair(-5.0, 5.0), false, false, 0.8); // 6min
		}
	}
};