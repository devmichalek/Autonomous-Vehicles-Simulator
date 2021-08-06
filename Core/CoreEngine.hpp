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
	bool load();

	// Execution loop
	void loop();

	// Execution of loop in case of error
	void errorLoop();
};