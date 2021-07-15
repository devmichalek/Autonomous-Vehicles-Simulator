#pragma once
#include "StateAbstract.hpp"

class StateTesting : public StateAbstract
{
public:
	StateTesting(StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void load();

	void draw();
};
