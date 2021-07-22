#pragma once
#include "StateAbstract.hpp"
#include "DrawableWallManager.hpp"
#include "DrawableFinishLine.hpp"

class StateTesting : public StateAbstract
{
	DrawableWallManager* m_wallManager;
	DrawableCar* m_car;
	DrawableFinishLine* m_finishLine;
	DrawableCarFactory m_carFactory;

public:
	StateTesting(StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void update();

	void load();

	void draw();
};
