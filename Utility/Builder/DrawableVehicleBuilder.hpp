#pragma once
#include "DrawableVehicle.hpp"

class DrawableVehicleBuilder
{
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	
	bool Validate();

public:

	void Clear();

	// Creates dummy drawable vehicle
	bool CreateDummy();

	// Returns drawable vehicle
	DrawableVehicle* Get();
};