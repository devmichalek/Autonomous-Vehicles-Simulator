#pragma once
#include "StateAbstract.hpp"
#include "DrawableManager.hpp"
#include "DrawableFinishLine.hpp"

class StateTesting : public StateAbstract
{
	DrawableManager* m_manager;
	DrawableCar* m_car;
	DrawableCarFactory m_carFactory;
	FitnessPoints m_fitnessPoints;

public:
	StateTesting(StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void update();

	void load();

	void draw();
};
