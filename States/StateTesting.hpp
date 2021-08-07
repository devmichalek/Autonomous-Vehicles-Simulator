#pragma once
#include "StateInterface.hpp"
#include "DrawableEdgeManager.hpp"
#include "DrawableCheckpointMap.hpp"

class StateTesting : public StateInterface
{
	DrawableEdgeManager* m_edgeManager;
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
