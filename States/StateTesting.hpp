#pragma once
#include "StateAbstract.hpp"

class StateTesting : public StateAbstract
{
public:
	StateTesting(StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void capture();

	void update();

	void load();

	void draw();
};
