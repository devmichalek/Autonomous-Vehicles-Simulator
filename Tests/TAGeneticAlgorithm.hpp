#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>
#include "AGeneticAlgorithm.hpp"

namespace TestGeneticAlgorithm
{
	void setSuccessColor()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 10);
	}

	void setAlmostSuccessColor()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 14);
	}

	void setErrorColor()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 12);
	}

	void setCommonColor()
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, 7);
	}

	void testCharacters()
	{
		
	}

	void testBooleans(GeneticFunctions::Abstract* geneticFunctions,
					  const size_t chromosomeLength,
					  const size_t populationSize,
					  const size_t numOfGenerations,
					  const double expectedResults)
	{
		std::cout << "\tTest parameters:\n";
		std::cout << "\t\tChromosome length: " << chromosomeLength << std::endl;
		std::cout << "\t\tPupulation size: " << populationSize << std::endl;
		std::cout << "\t\tNumber of generations: " << numOfGenerations << std::endl;
		auto start = std::chrono::high_resolution_clock::now();

		// Initialize
		GeneticAlgorithm<bool> geneticAlgorithm(geneticFunctions, chromosomeLength, populationSize);

		// Set up goal
		Chromosome<bool> goalChromosome;
		goalChromosome.resize(chromosomeLength); // All genes set to false

		// Environment
		std::vector<unsigned> points;
		points.resize(populationSize);
		for (size_t i = 0; i < numOfGenerations; ++i)
		{
			for (size_t j = 0; j < populationSize; ++j)
			{
				// Calculate fitness
				unsigned guessed = 0;
				for (size_t k = 0; k < chromosomeLength; ++k)
				{
					if (geneticAlgorithm[j][k].m_data == goalChromosome[k].m_data)
						++guessed;
				}

				points[j] = guessed;
			}

			geneticAlgorithm.regenerate(points);
		}

		double bestResults = double(*std::max_element(points.begin(), points.end())) / chromosomeLength;
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;

		const unsigned perfectResults = 100;
		auto precentBestResult = unsigned(bestResults * perfectResults);
		auto precentExpectedResults = unsigned(expectedResults * perfectResults);
		if (precentBestResult > precentExpectedResults || precentBestResult == perfectResults)
			setSuccessColor();
		else if (precentBestResult == precentExpectedResults)
			setAlmostSuccessColor();
		else
			setErrorColor();
		std::cout << "\tExpected to have " << expectedResults * perfectResults << "% of accuracy, ";
		std::cout << "after " << numOfGenerations << " generations received " << bestResults * 100 << "%\n";
		setCommonColor();
		std::cout << "\tElapsed time: " << elapsed.count() << "s\n\n";
	}

	void testFloatingPoints()
	{

	}

	void testIntegers()
	{

	}

	void runTests()
	{
		std::cout << "Test group name: TestGeneticAlgorithm\n";

		std::cout << "Test name: testCharacters\n";
		testCharacters();

		std::cout << "Test name: testBooleans\n";
		testBooleans(GeneticFunctions::boolean_45_10, 255, 64, 30, 0.75);
		testBooleans(GeneticFunctions::boolean_50_05, 200, 50, 30, 0.83);
		testBooleans(GeneticFunctions::boolean_60_03, 128, 32, 40, 0.94);
		testBooleans(GeneticFunctions::boolean_60_03, 64, 64, 64, 1.0);
		testBooleans(GeneticFunctions::boolean_50_02, 300, 128, 60, 0.95);
		testBooleans(GeneticFunctions::boolean_50_02, 2048, 128, 60, 0.70);
		testBooleans(GeneticFunctions::boolean_50_02, 1024, 256, 60, 0.80);
		testBooleans(GeneticFunctions::boolean_50_01, 512, 128, 60, 0.92);
		testBooleans(GeneticFunctions::boolean_50_01, 1024, 256, 50, 0.83);
		testBooleans(GeneticFunctions::boolean_50_01, 64, 64, 64, 1.0);

		std::cout << "Test name: testFloatingPoints\n";
		testFloatingPoints();

		std::cout << "Test name: testIntegers\n";
		testIntegers();
	}
};