#pragma once
#include <string>
#include <random>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "ArtificialNeuralNetwork.hpp"
#include "Genetic.hpp"

template <class Type>
struct Gene
{
	Type m_data;
	inline Gene<Type>& operator=(Gene<Type>& rhs) = default;
	inline Type operator=(Type rhs)
	{
		m_data = rhs;
		return m_data;
	}
};

template <class Type>
struct Chromosome
{
	std::vector<Gene<Type>> m_genes;

	Chromosome() = default;

	Chromosome(const size_t length)
	{
		m_genes.resize(length);
	}

	Chromosome(const Chromosome<Type>& rhs)
	{
		m_genes = rhs.m_genes;
	}

	~Chromosome()
	{
	}

	inline void resize(size_t length)
	{
		m_genes.resize(length);
	}

	inline Type& operator[](size_t i)
	{
		return m_genes[i].m_data;
	}

	inline size_t length()
	{
		return m_genes.size();
	}

	inline Chromosome<Type>& operator=(Chromosome<Type>& rhs)
	{
		std::memcpy(&m_genes[0], &rhs.m_genes[0], sizeof(Type) * length());
		return *this;
	}

	inline Chromosome<Type>& operator=(Chromosome<Type>&& rhs) noexcept
	{
		m_genes = rhs.m_genes;
		return *this;
	}

	template <class Type>
	friend std::ostream& operator<<(std::ostream& os, const Chromosome<Type>& rhs);
};

template <class Type>
class GeneticAlgorithm
{
protected:
	const size_t m_maxNumberOfGenerations;
	size_t m_currentIteration;
	const size_t m_chromosomeLength;
	const size_t m_populationSize;
	std::vector<Chromosome<Type>> m_population;
	double m_crossoverProbability;
	double m_mutationProbability;
	const size_t m_parentsCount;
	std::random_device m_randomDevice;
	std::mt19937 m_mersenneTwister;
	std::uniform_int_distribution<std::mt19937::result_type> m_coinDistribution;
	std::uniform_int_distribution<std::mt19937::result_type> m_hundredDistribution;

	virtual void mutate(Type&) = 0; // Mutate gene data
	virtual void crossover() // Recreate population based on parents
	{
		Chromosome<Type> newChromosome(m_chromosomeLength);
		for (size_t i = 0; i < m_chromosomeLength; ++i)
		{
			if (m_hundredDistribution(m_mersenneTwister) < (m_crossoverProbability * 100))
			{
				// Take gene from the first parent
				newChromosome[i] = m_population[0][i];
			}
			else
			{
				// Take gene from the second parent
				newChromosome[i] = m_population[1][i];
			}

			if (m_hundredDistribution(m_mersenneTwister) < (m_mutationProbability * 100))
				mutate(newChromosome[i]);
		}

		m_population.push_back(newChromosome);
	}

	virtual void select(const FitnessVector& points) // Select parents
	{
		// Deep copy
		FitnessVector dummy = points;

		// Find best ones
		std::vector<Chromosome<Type>> newPopulation;
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

public:
	GeneticAlgorithm(const size_t maxNumberOfGenerations,
					 const size_t chromosomeLength,
					 size_t populationSize,
					 const double crossoverProbability,
					 const double mutationProbability,
					 const unsigned parentsCount = 2) :
		m_maxNumberOfGenerations(maxNumberOfGenerations),
		m_currentIteration(0),
		m_chromosomeLength(chromosomeLength),
		m_populationSize(populationSize),
		m_crossoverProbability(crossoverProbability),
		m_mutationProbability(mutationProbability),
		m_parentsCount(parentsCount),
		m_mersenneTwister((std::random_device())()),
		m_coinDistribution(0, 1),
		m_hundredDistribution(0, 100)
	{
		assert(m_populationSize > m_parentsCount);
		assert(m_parentsCount <= 100);
		m_population.resize(m_populationSize);
		m_population.shrink_to_fit();
	}

	inline Chromosome<Type> getChromosome(const size_t index)
	{
		if (index < m_populationSize)
			return m_population[index];
		return Chromosome<Type>(m_chromosomeLength); // Error
	}

	bool iterate(const FitnessVector& points)
	{
		++m_currentIteration;
		if (m_currentIteration > m_maxNumberOfGenerations)
		{
			m_currentIteration = 0;
			return false;
		}

		select(points);

		size_t repeatCount = m_populationSize - m_parentsCount;
		for (size_t i = 0; i < repeatCount; ++i)
			crossover();
		m_population.shrink_to_fit();

		return true;
	}

	inline const size_t getChromosomeLength() const
	{
		return m_chromosomeLength;
	}

	inline const size_t getPopulationSize() const
	{
		return m_populationSize;
	}
};

class GeneticAlgorithmBoolean : public GeneticAlgorithm<bool>
{
	void mutate(bool& geneData)
	{
		geneData = !geneData;
	}

public:
	GeneticAlgorithmBoolean(const size_t maxNumberOfGenerations,
							const size_t chromosomeLength,
							const size_t populationSize,
							double crossoverProbability,
							double mutationProbability) :
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability)
	{
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
				j[i] = static_cast<bool>(m_coinDistribution(m_mersenneTwister));
		}
	}
};

class GeneticAlgorithmCharacter : public GeneticAlgorithm<char>
{
	bool m_decreaseMutationOverGenerations;
	std::string m_alphabet;
	std::uniform_int_distribution<std::mt19937::result_type> m_alphabetDistribution;

	void mutate(char& geneData)
	{
		size_t offset = m_alphabetDistribution(m_mersenneTwister);

		if (m_decreaseMutationOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentIteration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		size_t characterPosition = m_alphabet.find(geneData);
		characterPosition += offset;
		const size_t alphabetSize = m_alphabet.size();
		if (characterPosition >= alphabetSize)
			characterPosition -= alphabetSize;

		geneData = m_alphabet[characterPosition];
	}

public:
	GeneticAlgorithmCharacter(const size_t maxNumberOfGenerations,
							  const size_t chromosomeLength,
							  const size_t populationSize,
							  double crossoverProbability,
							  double mutationProbability,
							  std::string alphabet,
							  bool decreaseMutationOverGenerations) :
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability),
		m_alphabet(alphabet),
		m_decreaseMutationOverGenerations(decreaseMutationOverGenerations)
	{
		m_alphabetDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>((m_alphabet.size() - 1)));
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
				j[i] = m_alphabet[m_alphabetDistribution(m_mersenneTwister)];
		}
	}
};

class GeneticAlgorithmFloat : public GeneticAlgorithm<float>
{
	bool m_decreaseMutationOverGenerations;
	unsigned m_precision;
	std::pair<float, float> m_range;
	std::uniform_int_distribution<std::mt19937::result_type> m_rangeDistribution;

	void mutate(float& geneData)
	{
		size_t offset = m_rangeDistribution(m_mersenneTwister);

		if (m_decreaseMutationOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentIteration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		float finalOffset = float(offset) / m_precision;
		geneData += finalOffset;
		if (geneData > m_range.second)
			geneData = m_range.first + (geneData - m_range.second);
	}

public:

	GeneticAlgorithmFloat(const size_t maxNumberOfGenerations,
						  const size_t chromosomeLength,
						  const size_t populationSize,
						  double crossoverProbability,
						  double mutationProbability,
						  bool decreaseMutationOverGenerations,
						  unsigned precision,
						  std::pair<float, float> range) :
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability),
		m_decreaseMutationOverGenerations(decreaseMutationOverGenerations),
		m_precision(precision),
		m_range(range)
	{
		assert(m_range.first < m_range.second);
		float right = std::fabs(m_range.first - m_range.second);
		m_rangeDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>(right * m_precision));
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
			{
				float geneData = float(m_rangeDistribution(m_mersenneTwister)) / m_precision;
				if (geneData > m_range.second)
					geneData = m_range.first + (geneData - m_range.second);
				j[i] = geneData;
			}
		}
	}
};

class GeneticAlgorithmNeuron : public GeneticAlgorithm<Neuron>
{
	bool m_decreaseMutationOverGenerations;
	bool m_singlePointCrossover;
	size_t m_precision;
	std::pair<Neuron, Neuron> m_range;
	std::uniform_int_distribution<std::mt19937::result_type> m_rangeDistribution;

	void mutate(Neuron& geneData)
	{
		size_t offset = m_rangeDistribution(m_mersenneTwister);

		if (m_decreaseMutationOverGenerations)
		{
			double generationCompleteness = double(m_maxNumberOfGenerations - m_currentIteration) / m_maxNumberOfGenerations;
			offset = size_t(generationCompleteness * offset);
		}

		Neuron finalOffset = Neuron(offset) / m_precision;
		geneData += finalOffset;
		if (geneData > m_range.second)
			geneData = m_range.first + (geneData - m_range.second);
	}

public:
	GeneticAlgorithmNeuron(const size_t maxNumberOfGenerations,
						   const size_t chromosomeLength,
						   const size_t populationSize,
						   double crossoverProbability,
						   double mutationProbability,
						   bool decreaseMutationOverGenerations,
						   bool singlePointCrossover,
						   unsigned precision,
						   std::pair<Neuron, Neuron> range) :
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize, crossoverProbability, mutationProbability),
		m_decreaseMutationOverGenerations(decreaseMutationOverGenerations),
		m_singlePointCrossover(singlePointCrossover),
		m_precision(precision),
		m_range(range)
	{
		assert(m_range.first < m_range.second);
		Neuron right = std::fabs(m_range.first - m_range.second);
		m_rangeDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>(right * m_precision));
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
			{
				Neuron geneData = Neuron(m_rangeDistribution(m_mersenneTwister)) / m_precision;
				if (geneData > m_range.second)
					geneData = m_range.first + (geneData - m_range.second);
				j[i] = geneData;
			}
		}
	}

	Neuron* getIndividual(size_t identity)
	{
		if (identity >= m_populationSize)
			return nullptr;

		return reinterpret_cast<Neuron*>(&m_population[identity][0]);
	}
};

template <class Type>
std::ostream& operator<<(std::ostream& os, const Chromosome<Type>& rhs)
{
	if constexpr (std::is_same<Type, bool>::value)
	{
		auto rhsSize = rhs.m_genes.size();
		std::string rhsResult;
		rhsResult.resize(rhsSize);
		for (auto i = 0; i < rhsSize; ++i)
		{
			bool value = rhs.m_genes[i].m_data;
			rhsResult[i] = static_cast<char>(static_cast<unsigned>(value) + 48);
		}
		os << rhsResult;
	}
	else if constexpr (std::is_same<Type, char>::value)
	{
		auto rhsSize = rhs.m_genes.size();
		std::string rhsResult;
		rhsResult.resize(rhsSize);
		for (auto i = 0; i < rhsSize; ++i)
			rhsResult[i] = rhs.m_genes[i].m_data;
		os << rhsResult;
	}
	else if constexpr (std::is_same<Type, float>::value)
	{
		auto rhsSize = rhs.m_genes.size();
		os << std::fixed << std::setprecision(2) << rhs.m_genes[0].m_data;
		for (auto i = 1; i < rhsSize; ++i)
			os << ", " << std::fixed << std::setprecision(2) << rhs.m_genes[i].m_data;
	}
	else if constexpr (std::is_same<Type, double>::value)
	{
		auto rhsSize = rhs.m_genes.size();
		os << std::fixed << std::setprecision(4) << rhs.m_genes[0].m_data;
		for (auto i = 1; i < rhsSize; ++i)
			os << ", " << std::fixed << std::setprecision(4) << rhs.m_genes[i].m_data;
	}
	else
	{
		os << "There is no available chromosome representation.";
	}

	return os;
}