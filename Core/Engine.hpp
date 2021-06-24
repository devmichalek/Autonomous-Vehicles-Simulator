#pragma once
#include <vector>
#include "SAbstract.hpp"

class Engine final
{
	std::vector<State::Abstract*> m_states;

public:
	explicit Engine();
	~Engine();
	void load();
	void loop();
};