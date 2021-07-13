#pragma once
#include <vector>
#include "SAbstract.hpp"

class CoreEngine final
{
	std::vector<State::Abstract*> m_states;

public:
	explicit CoreEngine();
	~CoreEngine();
	void load();
	void loop();
};