#pragma once
#include "AbstractBuilder.hpp"
#include "DrawableVehicle.hpp"

class DrawableVehicleBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_TOO_LITTLE_VEHICLE_BODY_EDGES = LAST_ENUM_OPERATION_INDEX,
		ERROR_TOO_MANY_VEHICLE_BODY_EDGES,
		ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL,
		ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST,
		ERROR_TOO_LITTLE_SENSORS,
		ERROR_TOO_MANY_SENSORS,
		ERROR_SENSOR_ANGLE_IS_NOT_DIVISIBLE,
		ERROR_SENSOR_ANGLE_IS_TOO_LITTLE,
		ERROR_SENSOR_ANGLE_IS_TOO_LARGE,
	};

	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;

	// Validates number of vehicle body edges
	bool ValidateVehicleBodyNumberOfEdges(size_t count);

	// Calculates vehicle body area by given points representing triangles
	double CalculateVehicleBodyArea(std::vector<sf::Vector2f> points);

	// Validates vehicle body area
	bool ValidateVehicleBodyArea();
	
	// Validate vehicle body edges
	bool ValidateRelativePositions(const std::vector<sf::Vector2f>& points);

	// Validates number of sensors
	bool ValidateNumberOfSensors(size_t count);

	// Validates sensor's angle
	bool ValidateSensorAngle(double angle);

	// Validate internal fields
	bool ValidateInternal();

	// Clears internal fields
	void ClearInternal();

	// Sets internal fields from loaded file
	bool LoadInternal(std::ifstream& input);

	// Saves internal fields into the file
	bool SaveInternal(std::ofstream& output);
	
	// Creates vehicle body dummy
	VehicleBody CreateVehicleBodyDummy(sf::Vector2f dummySize);

public:

	DrawableVehicleBuilder();

	~DrawableVehicleBuilder();

	// Add part of vehicle body
	void AddVehicleBodyPoint(sf::Vector2f point);

	// Add vehicle sensor
	void AddVehicleSensor(sf::Vector2f point, double angle);

	// Creates drawable vehicle dummy
	bool CreateDummy();

	// Returns intermediate representation of vehicle body
	VehicleBody GetVehicleBody();

	// Returns intermediate representation of vehicle sensors
	VehicleSensors GetVehicleSensors();

	// Returns maximum vehicle body bound
	static sf::Vector2f GetMaxVehicleBodySize();

	// Returns minimum vehicle body number of edges
	static size_t GetMinVehicleBodyNumberOfEdges();

	// Returns maximum vehicle body number of edges
	static size_t GetMaxVehicleBodyNumberOfEdges();

	// Returns minimum sensor's angle
	static double GetMinSensorAngle();

	// Returns maximum sensor's angle
	static double GetMaxSensorAngle();

	// Returns minimum required number of sensors
	static size_t GetMinNumberOfSensors();

	// Returns maximum required number of sensors
	static size_t GetMaxNumberOfSensors();

	// Returns default sensor's angle offsets
	static double GetDefaultSensorAngleOffset();

	// Returns drawable vehicle
	DrawableVehicle* Get();
};