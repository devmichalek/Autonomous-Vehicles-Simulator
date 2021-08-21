#pragma once
#include "TestGeneticAlgorithm.hpp"

struct TestEngine
{
	TestEngine()
	{
		const bool runTestGeneticAlgorithm = false;

		if (runTestGeneticAlgorithm)
			TestGeneticAlgorithm::RunTests();
	}
};