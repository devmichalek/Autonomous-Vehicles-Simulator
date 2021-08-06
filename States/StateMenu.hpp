#pragma once
#include "StateAbstract.hpp"

class StateMenu : public StateAbstract
{
public:
	StateMenu(StateMenu&) = delete;

	StateMenu();

	~StateMenu();

	void capture();

	void update();

	bool load();

	void draw();
};
