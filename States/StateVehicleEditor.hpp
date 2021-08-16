#pragma once
#include "StateInterface.hpp"

class StateVehicleEditor final : public StateInterface
{
public:

	StateVehicleEditor(const StateVehicleEditor&) = delete;

	const StateVehicleEditor& operator=(const StateVehicleEditor&) = delete;

	StateVehicleEditor();

	~StateVehicleEditor();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};