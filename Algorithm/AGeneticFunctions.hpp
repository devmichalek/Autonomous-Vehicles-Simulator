#pragma once
#include <random>
#include "AGeneticStructures.hpp"

namespace GeneticFunctions
{
	// Rand machines
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<std::mt19937::result_type> coinDistribution(0, 1);
	std::uniform_int_distribution<std::mt19937::result_type> hundredDistribution(0, 99);

	class Abstract
	{
		// Mutate gene
		virtual void mutate(void*) = 0;

	public:
		virtual ~Abstract()
		{
		}

		virtual void initialize(void*) = 0;
		
		// Based on points selects best chromosomes as new parents
		virtual void select(void*, std::vector<unsigned>&) = 0;

		// Crossover chromosomes and create new population
		// Call select() function before calling crossover() function
		virtual void crossover(void*) = 0;
	};

	class Boolean final : public Abstract
	{
		inline std::vector<Chromosome<bool>>* convert(void* rhs)
		{
			return reinterpret_cast<std::vector<Chromosome<bool>>*>(rhs);
		}

		void mutate(void* rhs)
		{
			auto* gene = reinterpret_cast<Gene<bool>*>(rhs);
			gene->m_data = !gene->m_data;
		}

		size_t m_populationSize;
		double m_crossoverProbability;
		double m_mutationProbability;

		Chromosome<bool> spawn(size_t chromosomeLength)
		{
			Chromosome<bool> result;
			result.resize(chromosomeLength);
			for (auto& j : result.m_genes)
				j.m_data = static_cast<bool>(GeneticFunctions::coinDistribution(rng));
			return result;
		}

	public:
		Boolean(double crossoverProbability, double mutationProbability) : 
			m_crossoverProbability(crossoverProbability), m_mutationProbability(mutationProbability)
		{
			m_populationSize = 0;
		}

		void initialize(void* rhs)
		{
			auto* population = convert(rhs);
			
			for (auto i = population->begin(); i != population->end(); ++i)
			{
				// For each chromosome
				*i = spawn(i->length());
			}
		}

		void select(void* rhs, std::vector<unsigned>& points)
		{
			auto* population = convert(rhs);
			m_populationSize = population->size();

			// Select first parent
			auto bestChromosomeIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
			auto bestChromosome = *(population->begin() + bestChromosomeIndex);
			points[bestChromosomeIndex] = 0;

			// Select second parent
			auto secondBestChromosomeIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
			auto secondBestChromosome = *(population->begin() + secondBestChromosomeIndex);

			// Clear population
			population->clear();

			// Insert parents
			population->push_back(bestChromosome);
			population->push_back(secondBestChromosome);
		}

		void crossover(void* rhs)
		{
			auto* population = convert(rhs);
			size_t chromosomeLength = population->front().length();

			// Add new chromosomes
			auto repeat = m_populationSize - 2; // 2 - parents count
			while (repeat--)
			{
				Chromosome<bool> newChromosome;
				newChromosome.resize(chromosomeLength);
				
				for (size_t i = 0; i < chromosomeLength; ++i)
				{
					if (GeneticFunctions::hundredDistribution(rng) < m_crossoverProbability)
					{
						// Take gene from the first parent
						newChromosome[i] = (*population)[0][i];
					}
					else
					{
						// Take gene from the second parent
						newChromosome[i] = (*population)[1][i];
					}

					if (GeneticFunctions::hundredDistribution(rng) < m_mutationProbability)
					{
						// Mutate gene
						mutate((void*)&newChromosome[i]);
					}
				}

				population->push_back(newChromosome);
			}

			population->shrink_to_fit();
		}
	};

	// 45% chance of having gene from the first parent, 55% chance of having gene from the second parent
	// 10% chance of gene mutation
	Boolean* boolean_45_10 = new Boolean(45, 10);

	// 50% chance of having gene from the first parent, 50% chance of having gene from the second parent
	// 5% chance of gene mutation
	Boolean* boolean_50_05 = new Boolean(50, 5);

	// 60% chance of having gene from the first parent, 40% chance of having gene from the second parent
	// 3% chance of gene mutation
	Boolean* boolean_60_03 = new Boolean(60, 3);

	// 50% chance of having gene from the first parent, 50% chance of having gene from the second parent
	// 2% chance of gene mutation
	Boolean* boolean_50_02 = new Boolean(50, 2);

	// 50% chance of having gene from the first parent, 50% chance of having gene from the second parent
	// 1% chance of gene mutation
	Boolean* boolean_50_01 = new Boolean(50, 1);
}
