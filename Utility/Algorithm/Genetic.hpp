#pragma once

using Fitness = double;
using FitnessVector = std::vector<Fitness>;

enum : int
{
	UNIFORM_CROSSOVER,
	MEAN_CROSSOVER,
	ONE_POINT_CROSSOVER,
	TWO_POINT_CROSSOVER,
	NUMBER_OF_CROSSOVER_TYPES
};

const char* const crossoverTypeStrings[NUMBER_OF_CROSSOVER_TYPES] = {
	"Uniform crossover",
	"Mean crossover",
	"One point crossover",
	"Two point crossover"
};