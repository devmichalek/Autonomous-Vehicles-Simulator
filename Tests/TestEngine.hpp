#pragma once
#include "TestGeneticAlgorithm.hpp"

struct TestEngine
{
	TestEngine()
	{
		TestGeneticAlgorithm::runTests();
	}
};