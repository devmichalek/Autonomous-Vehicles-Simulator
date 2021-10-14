#pragma once

class SimulatedInterface
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

	SimulatedInterface(size_t category) :
		m_category(category)
	{
	}

	virtual ~SimulatedInterface()
	{
	}

	const size_t m_category;
};