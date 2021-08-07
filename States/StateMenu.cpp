#pragma once
#include "StateMenu.hpp"
#include "CoreConsoleLogger.hpp"

StateMenu::StateMenu()
{

}

StateMenu::~StateMenu()
{

}

void StateMenu::capture()
{

}

void StateMenu::update()
{

}

bool StateMenu::load()
{
	CoreConsoleLogger::PrintSuccess("State \"Menu\" dependencies loaded correctly");
	return true;
}

void StateMenu::draw()
{

}
