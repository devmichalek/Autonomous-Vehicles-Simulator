#pragma once

class SimulatedAbstract
{
public:

	enum
	{
		CategoryEdge = 0x002,
		CategoryCheckpoint = 0x004,
		CategoryVehicle = 0x008
	};

	// Returns category
	inline size_t GetCategory()
	{
		return m_category;
	}

protected:

	SimulatedAbstract(const size_t category) :
		m_category(category)
	{
	}

	const size_t m_category;
};