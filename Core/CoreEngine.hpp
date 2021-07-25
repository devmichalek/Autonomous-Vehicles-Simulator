#pragma once
#include <vector>
#include "StateAbstract.hpp"

class CoreEngine final
{
	// State container
	std::vector<StateAbstract*> m_states;

public:
	explicit CoreEngine();
	~CoreEngine();

	// Calls load function of each state
	void load();

	// Execution loop
	void loop();
};