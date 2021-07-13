#pragma once
#include <vector>
#include <string>
#include <random>
#include <iostream>

using FitnessPoint = unsigned;
using FitnessPoints = std::vector<FitnessPoint>;

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

	virtual void mutate(Type&) = 0; // Mutate gene data
	virtual void crossover() = 0; // Recreate population based on parents

	virtual void select(FitnessPoints& points) // Select parents
	{
		// Select first parent
		auto bestChromosomeIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
		auto bestChromosome = *(m_population.begin() + bestChromosomeIndex);
		points[bestChromosomeIndex] = 0;

		// Select second parent
		auto secondBestChromosomeIndex = std::distance(points.begin(), std::max_element(points.begin(), points.end()));
		auto secondBestChromosome = *(m_population.begin() + secondBestChromosomeIndex);

		// Clear population
		m_population.clear();

		// Insert parents
		m_population.push_back(bestChromosome);
		m_population.push_back(secondBestChromosome);
	}

public:
	GeneticAlgorithm(const size_t maxNumberOfGenerations,
					 const size_t chromosomeLength,
					 size_t populationSize) :
		m_maxNumberOfGenerations(maxNumberOfGenerations),
		m_currentIteration(0),
		m_chromosomeLength(chromosomeLength),
		m_populationSize(populationSize)
	{
		m_population.resize(m_populationSize);
		m_population.shrink_to_fit();
	}

	inline Chromosome<Type> getChromosome(const size_t index)
	{
		if (index < m_populationSize)
			return m_population[index];
		return Chromosome<Type>(m_chromosomeLength); // Error
	}

	bool iterate(FitnessPoints& points)
	{
		select(points);
		crossover();

		++m_currentIteration;
		if (m_currentIteration > m_maxNumberOfGenerations)
		{
			m_currentIteration = 0;
			return false;
		}

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

namespace Distribution
{
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<std::mt19937::result_type> coin(0, 1);
	std::uniform_int_distribution<std::mt19937::result_type> hundred(0, 99);

	inline const int randCoin()
	{
		return coin(rng);
	}

	inline const int randHundred()
	{
		return hundred(rng);
	}
}

class GeneticAlgorithmBoolean : public GeneticAlgorithm<bool>
{
	double m_crossoverProbability;
	double m_mutationProbability;

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
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize),
		m_crossoverProbability(crossoverProbability),
		m_mutationProbability(mutationProbability)
	{
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
				j[i] = static_cast<bool>(Distribution::randCoin());
		}
	}

	void crossover()
	{
		// Add new chromosomes
		auto repeat = m_populationSize - 2; // 2 - parents count
		while (repeat--)
		{
			Chromosome<bool> newChromosome(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
			{
				if (Distribution::randHundred() < (m_crossoverProbability * 100))
				{
					// Take gene from the first parent
					newChromosome[i] = m_population[0][i];
				}
				else
				{
					// Take gene from the second parent
					newChromosome[i] = m_population[1][i];
				}

				if (Distribution::randHundred() < (m_mutationProbability * 100))
					mutate(newChromosome[i]);
			}

			m_population.push_back(newChromosome);
		}

		m_population.shrink_to_fit();
	}
};

class GeneticAlgorithmCharacter : public GeneticAlgorithm<char>
{
	double m_crossoverProbability;
	double m_mutationProbability;
	bool m_decreaseMutationOverGenerations;
	std::string m_alphabet;
	std::uniform_int_distribution<std::mt19937::result_type> m_alphabetDistribution;

	void mutate(char& geneData)
	{
		size_t offset = m_alphabetDistribution(Distribution::rng);

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
		GeneticAlgorithm(maxNumberOfGenerations, chromosomeLength, populationSize),
		m_crossoverProbability(crossoverProbability),
		m_mutationProbability(mutationProbability),
		m_alphabet(alphabet),
		m_decreaseMutationOverGenerations(decreaseMutationOverGenerations)
	{
		m_alphabetDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, static_cast<int>((m_alphabet.size() - 1)));
		for (auto& j : m_population)
		{
			j.resize(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
				j[i] = m_alphabet[m_alphabetDistribution(Distribution::rng)];
		}
	}

	void crossover()
	{
		// Add new chromosomes
		auto repeat = m_populationSize - 2; // 2 - parents count
		while (repeat--)
		{
			Chromosome<char> newChromosome(m_chromosomeLength);
			for (size_t i = 0; i < m_chromosomeLength; ++i)
			{
				if (Distribution::randHundred() < (m_crossoverProbability * 100))
				{
					// Take gene from the first parent
					newChromosome[i] = m_population[0][i];
				}
				else
				{
					// Take gene from the second parent
					newChromosome[i] = m_population[1][i];
				}

				if (Distribution::randHundred() < (m_mutationProbability * 100))
					mutate(newChromosome[i]);
			}

			m_population.push_back(newChromosome);
		}

		m_population.shrink_to_fit();
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
	else
	{
		os << "There is no available chromosome representation.";
	}

	return os;
}