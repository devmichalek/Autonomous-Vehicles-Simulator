#pragma once
#include <vector>
#include "StateAbstract.hpp"

class CoreEngine final
{
	std::vector<StateAbstract*> m_states;

public:
	explicit CoreEngine();
	~CoreEngine();
	void load();
	void loop();
};