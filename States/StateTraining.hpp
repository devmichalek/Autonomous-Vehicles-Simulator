#pragma once
#include "StateAbstract.hpp"

class StateTraining : public StateAbstract
{
public:
	StateTraining(StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void load();

	void draw();
};
