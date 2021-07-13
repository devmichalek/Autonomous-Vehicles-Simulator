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

		void finish()
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

		void stop()
		{
			if (m_running)
			{
				m_running = false;
				finish();
			}
		}

		~TestTimer()
		{
			stop();
		}
	};

	template<class Type>
	void printTestStatistics(const size_t numOfGenerations,
							 const double expectedResults,
							 const double results,
							 Chromosome<Type>& expectedChromosome,
							 Chromosome<Type>& bestChromosome)
	{
		const unsigned perfectResults = 100;
		auto precentBestResult = unsigned(results * perfectResults);
		auto precentExpectedResults = unsigned(expectedResults * perfectResults);

		std::cout << "\tExpected: " << expectedChromosome << std::endl;
		std::cout << "\tResult:   " << bestChromosome << std::endl;

		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (precentBestResult > precentExpectedResults || precentBestResult == perfectResults)
			SetConsoleTextAttribute(handle, 10);
		else if (precentBestResult == precentExpectedResults)
			SetConsoleTextAttribute(handle, 14);
		else
			SetConsoleTextAttribute(handle, 12);

		std::cout << "\tExpected to have " << expectedResults * perfectResults << "% of accuracy, ";
		std::cout << "after " << numOfGenerations << " generations received " << results * 100 << "%\n\n";
		SetConsoleTextAttribute(handle, 7);
	}

	void testBooleans(const size_t chromosomeLength,
					  const size_t populationSize,
					  const size_t numOfGenerations,
					  double crossoverProbability,
					  double mutationProbability,
					  const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		std::cout << "\t\tCrossover probability: " << crossoverProbability << std::endl;
		std::cout << "\t\tMutation probability: " << mutationProbability << std::endl;

		// Set up points vector
		FitnessPoints points(populationSize);

		// Initialize
		GeneticAlgorithmBoolean geneticAlgorithm(numOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability);

		// Set up goal
		Chromosome<bool> expectedChromosome(chromosomeLength); // All genes set to false

		// Run environment
		TestTimer testTimer;
		do
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				points[j] = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					if (geneticAlgorithm.getChromosome(j)[k] == expectedChromosome[k])
						++points[j];
				}
			}
		} while (geneticAlgorithm.iterate(points));
		testTimer.stop();

		size_t bestResultsIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
		double bestResults = double(points[bestResultsIndex]) / chromosomeLength;
		auto bestChromosome = geneticAlgorithm.getChromosome(bestResultsIndex);
		printTestStatistics<bool>(numOfGenerations, expectedResults, bestResults, expectedChromosome, bestChromosome);
	}

	void testCharacters(const size_t chromosomeLength,
						const size_t populationSize,
						const size_t numOfGenerations,
						double crossoverProbability,
						double mutationProbability,
						std::string alphabet,
						bool decreaseMutationOverGenerations,
						const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		std::cout << "\t\tCrossover probability: " << crossoverProbability << std::endl;
		std::cout << "\t\tMutation probability: " << mutationProbability << std::endl;
		std::cout << "\t\tDecrease mutation over generations: " << decreaseMutationOverGenerations << std::endl;

		// Set up points vector
		FitnessPoints points(populationSize);

		// Initialize
		GeneticAlgorithmCharacter geneticAlgorithm(numOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability, alphabet, decreaseMutationOverGenerations);
		const size_t numOfCombinations = alphabet.size();

		// Set up goal, all genes set to 'X' character
		Chromosome<char> expectedChromosome(chromosomeLength);
		for (size_t i = 0; i < chromosomeLength; ++i)
			expectedChromosome[i] = 'X';

		// Run environment
		TestTimer testTimer;
		while (geneticAlgorithm.iterate(points))
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				points[j] = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					size_t difference = std::abs(geneticAlgorithm.getChromosome(j)[k] - expectedChromosome[k]);
					difference = size_t(std::pow(difference, 2));
					long long value = long long(numOfCombinations) - difference;
					points[j] += unsigned(value < 0 ? 0 : value);
				}
			}
		}
		testTimer.stop();

		size_t bestResultsIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
		double bestResults = double(points[bestResultsIndex]) / chromosomeLength / numOfCombinations;
		auto bestChromosome = geneticAlgorithm.getChromosome(bestResultsIndex);
		printTestStatistics<char>(numOfGenerations, expectedResults, bestResults, expectedChromosome, bestChromosome);
	}

	//void testFloatingPoints(const size_t chromosomeLength,
	//						const size_t populationSize,
	//						const size_t numOfGenerations,
	//						double crossoverProbability,
	//						double mutationProbability,
	//						const double expectedResults)
	//{
	//	std::cout << "\tTest parameters:\n";
	//	std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
	//	std::cout << "\t\tPupulation size: " << populationSize << std::endl;
	//	std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;

	//	// Set up points vector
	//	FitnessPoints points(populationSize);

	//	// Initialize
	//	GeneticAlgorithm<float> geneticAlgorithm(geneticFunctions, chromosomeLength, populationSize);

	//	// Set up goal, all genes set to 1.0
	//	Chromosome<float> expectedChromosome(chromosomeLength);
	//	for (size_t i = 0; i < chromosomeLength; ++i)
	//		expectedChromosome[i] = 5.0;

	//	// Run environment
	//	{
	//		// Set up timer
	//		TestTimer testTimer;

	//		for (size_t i = 0; i < numOfGenerations; ++i)
	//		{
	//			for (size_t j = 0; j < populationSize; ++j)
	//			{
	//				// Calculate fitness
	//				points[j] = 0;
	//				for (size_t k = 0; k < chromosomeLength; ++k)
	//				{
	//					/*size_t difference = std::abs(geneticAlgorithm[j][k] - expectedChromosome[k]);
	//					difference = std::pow(difference, 2);
	//					long long value = long long(numOfCombinations) - difference;
	//					points[j] += size_t(value < 0 ? 0 : value);*/
	//				}
	//			}

	//			geneticAlgorithm.regenerate(points);
	//		}
	//	}

	//	size_t bestResultsIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
	//	double bestResults = double(points[bestResultsIndex]) / chromosomeLength;
	//	printTestStatistics<float>(numOfGenerations, expectedResults, bestResults, expectedChromosome, geneticAlgorithm[bestResultsIndex]);
	//}

	void testIntegers()
	{

	}

	void runTests()
	{
		std::cout << "Test title: TestGeneticAlgorithm\n";

		const bool runTestGroupBooleans = false;
		const bool runTestGroupCharacters = true;
		const bool runTestGroupFloatingPoints = true;

		if (runTestGroupBooleans)
		{
			std::cout << "Test group name: testBooleans\n";
			testBooleans(255, 64, 30, 0.45, 0.1, 0.75);
			testBooleans(200, 50, 30, 0.5, 0.05, 0.83);
			testBooleans(128, 32, 40, 0.6, 0.03, 0.94);
			testBooleans(64, 64, 64, 0.6, 0.03, 1.0);
			testBooleans(300, 128, 60, 0.5, 0.02, 0.95);
			testBooleans(2048, 128, 60, 0.5, 0.02, 0.70);
			testBooleans(1024, 256, 60, 0.5, 0.02, 0.80);
			testBooleans(512, 128, 60, 0.5, 0.01, 0.92);
			testBooleans(1024, 256, 50, 0.5, 0.01, 0.83);
			testBooleans(64, 64, 64, 0.5, 0.01, 1.0);
		}
		
		if (runTestGroupCharacters)
		{
			std::cout << "Test group name: testCharacters\n";
			const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}";
			testCharacters(20, 128, 30, 0.5, 0.1, alphabet, false, 0.9);
			testCharacters(32, 128, 60, 0.6, 0.08, alphabet, false, 0.9);
			testCharacters(128, 256, 90, 0.5, 0.05, alphabet, false, 0.8);
			testCharacters(256, 512, 512, 0.5, 0.03, alphabet, true, 0.94);
			testCharacters(192, 512, 210, 0.5, 0.03, alphabet, false, 0.92);
			testCharacters(256, 1024, 512, 0.5, 0.03, alphabet, false, 0.9);
		}

		if (runTestGroupFloatingPoints)
		{
			std::cout << "Test group name: testFloatingPoints\n";
			//testFloatingPoints();
		}

		std::cout << "Test group name: testIntegers\n";
		testIntegers();
	}
};