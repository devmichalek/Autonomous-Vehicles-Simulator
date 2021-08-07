#pragma once
#include "StateInterface.hpp"

class StateMenu : public StateInterface
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
