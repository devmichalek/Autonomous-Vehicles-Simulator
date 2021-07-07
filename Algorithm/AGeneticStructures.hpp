#pragma once
#include <vector>

template <class Type>
struct Gene
{
	Type m_data;
};

template <class Type>
struct Chromosome
{
	std::vector<Gene<Type>> m_genes;

	Chromosome()
	{
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

	inline Gene<Type>& operator[](size_t i)
	{
		return m_genes[i];
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

	inline Chromosome<Type>& operator=(Chromosome<Type>&& rhs)
	{
		m_genes = rhs.m_genes;
		return *this;
	}
};
