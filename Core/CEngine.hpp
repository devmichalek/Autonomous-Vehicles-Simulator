#pragma once
#include <vector>
#include "SAbstract.hpp"

class CEngine final
{
	std::vector<State::Abstract*> m_states;

public:
	explicit CEngine();
	~CEngine();
	void load();
	void loop();
};