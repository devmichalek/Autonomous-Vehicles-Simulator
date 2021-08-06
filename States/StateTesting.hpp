#pragma once
#include "StateAbstract.hpp"
#include "DrawableManager.hpp"
#include "DrawableCheckpointMap.hpp"

class StateTesting : public StateAbstract
{
	DrawableManager* m_manager;
	DrawableCar* m_car;
	DetailedCarFactory m_carFactory;
	DrawableCheckpointMap* m_checkpointMap;

public:
	StateTesting(StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void update();

	bool load();

	void draw();
};
