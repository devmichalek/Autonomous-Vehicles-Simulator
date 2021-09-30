#pragma once
#include <string>
#include <random>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "ArtificialNeuralNetwork.hpp"
#include "Genetic.hpp"

template <class Type>
class GeneticAlgorithm
{
protected:

	using Gene = Type;
	using Chromosome = std::vector<Gene>;
	const size_t m_maxNumberOfGenerations;
	size_t m_currentGeneration;
	const size_t m_chromosomeLength;
	const size_t m_populationSize;
	std::vector<Chromosome> m_population;
	const int m_crossoverType;
	const bool m_repeatCrossoverPerIndividual;
	const double m_mutationProbability;
	const bool m_decreaseMutationProbabilityOverGenerations;
	const size_t m_parentsCount;
	static inline std::mt19937 m_mersenneTwister = std::mt19937((std::random_device())());
	static inline std::uniform_int_distribution<std::mt19937::result_type> m_hundredDistribution =
		std::uniform_int_distribution<std::mt19937::result_type>(0, 100);

	// Creates new chromosome (individual) based on parents
	Chromosome Crossover()
	{
		Chromosome newChromosome(m_chromosomeLength);
		switch (m_crossoverType)
		{
			case UNIFORM_CROSSOVER:
			{
				const size_t probabilityPerParent = 100 / m_parentsCount;
				for (size_t i = 0; i < m_chromosomeLength; ++i)
				{
					size_t parentIndex = m_hundredDistribution(m_mersenneTwister) % probabilityPerParent;
					if (parentIndex >= m_parentsCount)
						parentIndex = m_parentsCount - 1;
					newChromosome[i] = m_population[parentIndex][i];
				}
				break;
			}
			case MEAN_CROSSOVER:
			{
				for (size_t i = 0; i < m_chromosomeLength; ++i)
				{
					Gene gene = m_population[0][i];
					for (size_t j = 1; j < m_parentsCount; ++j)
						gene += m_population[j][i];
					newChromosome[i] = gene / Gene(m_parentsCount);
				}
				break;
			}
			case ONE_POINT_CROSSOVER:
			{
				const double percentage = double(m_hundredDistribution(m_mersenneTwister)) / 100.0;
				const size_t pivot = size_t(percentage * m_chromosomeLength);
				for (size_t i = 0; i < pivot; ++i)
					newChromosome[i] = m_population[0][i];
				for (size_t i = pivot; i < m_chromosomeLength; ++i)
					newChromosome[i] = m_population[1][i];
				break;
			}
			case TWO_POINT_CROSSOVER:
			default:
			{
				const double percentage1 = double(m_hundredDistribution(m_mersenneTwister)) / 100.0;
				const size_t pivot1 = size_t(percentage1 * m_chromosomeLength);
				const double percentage2 = double(m_hundredDistribution(m_mersenneTwister)) / 100.0;
				const size_t pivot2 = pivot1 + size_t(percentage2 * (m_chromosomeLength - pivot1));
				for (size_t i = 0; i < pivot1; ++i)
					newChromosome[i] = m_population[0][i];
				for (size_t i = pivot1; i < pivot2; ++i)
					newChromosome[i] = m_population[1][i];
				for (size_t i = pivot2; i < m_chromosomeLength; ++i)
					newChromosome[i] = m_population[0][i];
				break;
			}
		}

		return newChromosome;
	}

private:

	// Selects best chromosomes as parents
	virtual void Select(const FitnessVector& points)
	{
		// Deep copy
		FitnessVector dummy = points;

		// Find best ones
		std::vector<Chromosome> newPopulation;
		for (size_t i = 0; i < m_parentsCount; ++i)
		{
			// Select parent
			auto bestChromosomeIndex = std::distance(dummy.begin(), std::max_element(dummy.begin(), dummy.end()));
			dummy[bestChromosomeIndex] = 0;
			newPopulation.push_back(m_population[bestChromosomeIndex]);
		}

		// New population consists of fittest chromosomes
		m_population = newPopulation;
	}

	// Mutates gene
	virtual void Mutate(Gene& gene) = 0;

public:

	GeneticAlgorithm(const size_t maxNumberOfGenerations,
					 const size_t chromosomeLength,
					 const size_t populationSize,
					 const int crossoverType,
					 const bool repeatCrossoverPerIndividual,
					 const double mutationProbability,
					 const bool decreaseMutationProbabilityOverGenerations = false,
					 const unsigned parentsCount = 2) :
		m_maxNumberOfGenerations(maxNumberOfGenerations),
		m_currentGeneration(0),
		m_chromosomeLength(chromosomeLength),
		m_populationSize(populationSize),
		m_crossoverType(crossoverType),
		m_repeatCrossoverPerIndividual(repeatCrossoverPerIndividual),
		m_mutationProbability(mutationProbability),
		m_decreaseMutationProbabilityOverGenerations(decreaseMutationProbabilityOverGenerations),
		m_parentsCount(parentsCount)
	{
		assert(m_populationSize > m_parentsCount);
		m_population.resize(m_populationSize);
		m_population.shrink_to_fit();
	}

	inline size_t GetCurrentGeneration() const
	{
		return m_currentGeneration;
	}

	inline Chromosome GetIndividualChromosome(const size_t identity) const
	{
		if (identity < m_populationSize)
			return m_population[identity];
		return Chromosome(m_chromosomeLength);
	}

	inline Gene* GetIndividualGenes(const size_t identity)
	{
		if (identity >= m_populationSize)
			return nullptr;
		return &m_population[identity][0];
	}

	bool Iterate(const FitnessVector& points)
	{
		++m_currentGeneration;
		if (m_currentGeneration > m_maxNumberOfGenerations)
		{
			m_currentGeneration = 0;
			return false;
		}

		Select(points);

		const size_t repeatCount = m_populationSize - m_parentsCount;
		if (m_repeatCrossoverPerIndividual)
		{
			for (size_t i = 0; i < repeatCount; ++i)
			{
				auto individual = Crossover();
				for (auto& gene : individual)
				{
					if (m_hundredDistribution(m_mersenneTwister) < (m_mutationProbability * 100))
						Mutate(gene);
				}
				m_population.push_back(individual);
			}
		}
		else
		{
			auto dummy = Crossover();
			for (size_t i = 0; i < repeatCount; ++i)
			{
				auto individual = dummy;
				for (auto& gene : individual)
				{
					if (m_hundredDistribution(m_mersenneTwister) < (m_mutationProbability * 100))
						Mutate(gene);
				}
				m_population.push_back(individual);
			}
		}
		
		m_population.shrink_to_fit();
		return true;
	}

	inline const size_t GetChromosomeLength() const
	{
		return m_chromosomeLength;
	}

	inline const size_t GetPopulationSize() const
	{
		return m_populationSize;
	}
};

class GeneticAlgorithmCharacter : public GeneticAlgorithm<char>
{
	const std::string m_alphabet;
	std::uniform_int_distribution<std::mt19937::result_type> m_alphabetDistribution;

	void Mutate(char& gene)
	{
		size_t offset = m_alphabetDistribution(m_mersenneTwister);

		if (m_decreaseMutationProbabilityOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentGeneration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		size_t characterPosition = m_alphabet.find(gene);
		characterPosition += offset;
		const size_t alphabetSize = m_alphabet.size();
		if (characterPosition >= alphabetSize)
			characterPosition -= alphabetSize;

		gene = m_alphabet[characterPosition];
	}

public:

	GeneticAlgorithmCharacter(const size_t maxNumberOfGenerations,
							  const size_t chromosomeLength,
							  const size_t populationSize,
							  const int crossoverType,
							  const bool repeatCrossoverPerIndividual,
							  const double mutationProbability,
							  const bool decreaseMutationProbabilityOverGenerations,
							  const std::string alphabet) :
		GeneticAlgorithm(maxNumberOfGenerations,
						 chromosomeLength,
						 populationSize,
						 crossoverType,
						 repeatCrossoverPerIndividual,
						 mutationProbability,
						 decreaseMutationProbabilityOverGenerations),
		m_alphabet(alphabet)
	{
		m_alphabetDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>((m_alphabet.size() - 1)));
		for (auto& individual : m_population)
		{
			individual.resize(m_chromosomeLength);
			for (auto& gene : individual)
				gene = m_alphabet[m_alphabetDistribution(m_mersenneTwister)];
		}
	}
};

class GeneticAlgorithmFloat : public GeneticAlgorithm<float>
{
	unsigned m_precision;
	std::pair<float, float> m_range;
	std::uniform_int_distribution<std::mt19937::result_type> m_rangeDistribution;

	void Mutate(float& gene)
	{
		size_t offset = m_rangeDistribution(m_mersenneTwister);

		if (m_decreaseMutationProbabilityOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentGeneration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		float finalOffset = float(offset) / m_precision;
		gene += finalOffset;
		if (gene > m_range.second)
			gene = m_range.first + (gene - m_range.second);
	}

public:

	GeneticAlgorithmFloat(const size_t maxNumberOfGenerations,
						  const size_t chromosomeLength,
						  const size_t populationSize,
						  const int crossoverType,
						  const bool repeatCrossoverPerIndividual,
						  const double mutationProbability,
						  const bool decreaseMutationProbabilityOverGenerations,
						  const unsigned precision,
						  const std::pair<float, float> range) :
		GeneticAlgorithm(maxNumberOfGenerations,
						 chromosomeLength,
						 populationSize,
						 crossoverType,
						 repeatCrossoverPerIndividual,
						 mutationProbability,
						 decreaseMutationProbabilityOverGenerations),
		m_precision(precision),
		m_range(range)
	{
		assert(m_range.first < m_range.second);
		float right = std::fabs(m_range.first - m_range.second);
		m_rangeDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>(right * m_precision));
		for (auto& individual : m_population)
		{
			individual.resize(m_chromosomeLength);
			for (auto& gene : individual)
			{
				float newGene = float(m_rangeDistribution(m_mersenneTwister)) / m_precision;
				if (newGene > m_range.second)
					newGene = m_range.first + (newGene - m_range.second);
				gene = newGene;
			}
		}
	}
};

class GeneticAlgorithmNeuron : public GeneticAlgorithm<Neuron>
{
	size_t m_precision;
	std::pair<Neuron, Neuron> m_range;
	std::uniform_int_distribution<std::mt19937::result_type> m_rangeDistribution;

	void Mutate(Neuron& gene)
	{
		size_t offset = m_rangeDistribution(m_mersenneTwister);

		if (m_decreaseMutationProbabilityOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentGeneration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		Neuron finalOffset = Neuron(offset) / m_precision;
		gene += finalOffset;
		if (gene > m_range.second)
			gene = m_range.first + (gene - m_range.second);
	}

public:

	GeneticAlgorithmNeuron(const size_t maxNumberOfGenerations,
						   const size_t chromosomeLength,
						   const size_t populationSize,
						   const int crossoverType,
						   const bool repeatCrossoverPerIndividual,
						   double mutationProbability,
						   bool decreaseMutationProbabilityOverGenerations,
						   const unsigned parentsCount,
						   unsigned precision,
						   std::pair<Neuron, Neuron> range) :
		GeneticAlgorithm(maxNumberOfGenerations,
						 chromosomeLength,
						 populationSize,
						 crossoverType,
						 repeatCrossoverPerIndividual,
						 mutationProbability,
						 decreaseMutationProbabilityOverGenerations,
						 parentsCount),
		m_precision(precision),
		m_range(range)
	{
		assert(m_range.first < m_range.second);
		Neuron right = std::fabs(m_range.first - m_range.second);
		m_rangeDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>(right * m_precision));
		for (auto& individual : m_population)
		{
			individual.resize(m_chromosomeLength);
			for (auto& gene : individual)
			{
				Neuron newGene = Neuron(m_rangeDistribution(m_mersenneTwister)) / m_precision;
				if (newGene > m_range.second)
					newGene = m_range.first + (newGene - m_range.second);
				gene = newGene;
			}
		}
	}
};
