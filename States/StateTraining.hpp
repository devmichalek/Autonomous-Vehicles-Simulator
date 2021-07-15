#pragma once
#include "StateAbstract.hpp"

class StateTraining : public StateAbstract
{
public:
	StateTraining(StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void capture();

	void update();

	void load();

	void draw();
};
