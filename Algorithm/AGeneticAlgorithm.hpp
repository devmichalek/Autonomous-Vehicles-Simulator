#pragma once
#include "AGeneticFunctions.hpp"

template <class Type>
class GeneticAlgorithm
{
	std::vector<Chromosome<Type>> m_population;
	GeneticFunctions::Abstract* m_functions;

public:
	GeneticAlgorithm(GeneticFunctions::Abstract* functions, size_t chromosomeLength,  size_t populationSize)
	{
		m_population.resize(populationSize);
		for (auto &i : m_population)
			i.resize(chromosomeLength);

		m_functions = functions;
		m_functions->initialize((void*)&m_population);
	}

	inline Chromosome<Type>& operator[](size_t i)
	{
		return m_population[i];
	}

	void regenerate(std::vector<unsigned>& points)
	{
		m_functions->select((void*)&m_population, points);
		m_functions->crossover((void*)&m_population);
	}

	const size_t getChromosomeLength() const
	{
		return m_population.size();
	}

	const size_t getPopulationSize() const
	{
		return m_population.front().m_genes.size();
	}
};