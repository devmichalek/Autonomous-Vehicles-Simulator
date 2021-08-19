#pragma once
#include "DrawableVehicle.hpp"

class DrawableVehicleBuilder final
{
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	bool m_validated;

	// Validates internal fields
	bool Validate();

public:

	// Clears internal fields
	void Clear();

	// Add part of vehicle body
	void AddVehicleBody(sf::Vector2f point);

	// Add vehicle sensor
	void AddVehicleSensor(sf::Vector2f point, double angle);

	// Loads map from file
	bool Load(std::string filename);

	// Saves map to file
	bool Save(std::string filename);

	// Creates dummy drawable vehicle
	bool CreateDummy();

	// Returns intermediate representation of vehicle body
	VehicleBody GetVehicleBody();

	// Returns intermediate representation of vehicle sensors
	VehicleSensors GetVehicleSensors();

	// Returns maximum vehicle body bound
	sf::Vector2f GetMaxVehicleBodySize();

	// Returns drawable vehicle
	DrawableVehicle* Get();
};