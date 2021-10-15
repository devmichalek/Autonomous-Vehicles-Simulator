#pragma once
#include "AbstractBuilder.hpp"
#include "VehiclePrototype.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"

class VehicleBuilder final :
	public AbstractBuilder
{
	enum
	{
		ERROR_TOO_LITTLE_VEHICLE_BODY_POINTS = LAST_ENUM_OPERATION_INDEX,
		ERROR_TOO_MANY_VEHICLE_BODY_POINTS,
		ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL,
		ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST,
		ERROR_TOO_LITTLE_SENSORS,
		ERROR_TOO_MANY_SENSORS,
		ERROR_SENSOR_ANGLE_IS_NOT_DIVISIBLE,
		ERROR_SENSOR_ANGLE_IS_TOO_LITTLE,
		ERROR_SENSOR_ANGLE_IS_TOO_LARGE,
		ERROR_SENSOR_MOTION_RANGE_IS_TOO_LITTLE,
		ERROR_SENSOR_MOTION_RANGE_IS_TOO_LARGE,
		ERROR_SENSOR_IS_OUTSIDE_OF_VEHICLE_BODY
	};

	VehiclePrototype m_vehiclePrototype;
	static sf::Vector2f m_maxVehicleBodyBound;
	static sf::Vector2f m_minVehicleBodyBound;
	static double m_defaultVehicleBeamLength;
	static sf::Vector2f m_defaultVehicleSensorSize;
	static float m_maxVehicleMass;

	// Validates number of vehicle body points
	bool ValidateVehicleBodyNumberOfPoints(size_t count);

	// Validates vehicle body area
	bool ValidateVehicleBodyArea();
	
	// Validate vehicle body edges
	bool ValidateRelativePositions(const std::vector<sf::Vector2f>& points);

	// Validates number of sensors
	bool ValidateNumberOfSensors(size_t count);

	// Validates sensor's angle
	bool ValidateSensorAngle(double angle);

	// Validates sensor's motion range
	bool ValidateSensorMotionRange(double motionRange);

	// Validates sensor's position over vehicle body
	bool ValidateSensorPositionsOverVehicleBody();

	// Validate internal fields
	bool ValidateInternal();

	// Clears internal fields
	void ClearInternal();

	// Sets internal fields from loaded file
	bool LoadInternal(std::ifstream& input);

	// Saves internal fields into the file
	bool SaveInternal(std::ofstream& output);

	// Creates vehicle prototype dummy
	void CreateDummyInternal();
	
public:

	VehicleBuilder();

	~VehicleBuilder();

	// Adds part of vehicle body
	inline void AddVehicleBodyPoint(sf::Vector2f point)
	{
		m_vehiclePrototype.AddBodyPoint(point);
	}

	// Adds vehicle sensor
	void AddVehicleSensor(sf::Vector2f point, double angle, double motionRange)
	{
		m_vehiclePrototype.AddSensor(point, angle, motionRange);
	}

	// Returns maximum vehicle body bound
	inline static sf::Vector2f GetMaxVehicleBodyBound()
	{
		return m_maxVehicleBodyBound;
	}

	// Returns minimum vehicle body bound
	inline static sf::Vector2f GetMinVehicleBodyBound()
	{
		return m_minVehicleBodyBound;
	}

	// Returns minimum vehicle body number of points
	inline static size_t GetMinNumberOfBodyPoints()
	{
		return 3;
	}

	// Returns maximum vehicle body number of points
	inline static size_t GetMaxNumberOfBodyPoints()
	{
		return b2_maxPolygonVertices;
	}

	// Returns minimum sensor's angle
	inline static double GetMinSensorAngle()
	{
		return 0.0;
	}

	// Returns maximum sensor's angle
	inline static double GetMaxSensorAngle()
	{
		return 360.0;
	}

	// Returns minimum sensor's motion range
	inline static double GetMinSensorMotionRange()
	{
		return 0.0;
	}

	// Returns maximum sensor's motion range
	inline static double GetMaxSensorMotionRange()
	{
		return 10.0;
	}

	// Returns default sensor's motion range
	inline static double GetDefaultSensorMotionRange()
	{
		return 1.0;
	}

	// Returns sensor's motion range multiplier
	inline static double GetSensorMotionRangeMultiplier()
	{
		return 32.0;
	}

	// Returns minimum required number of sensors
	inline static size_t GetMinNumberOfSensors()
	{
		return ArtificialNeuralNetworkBuilder::GetMinNumberOfNeuronsPerLayer();
	}

	// Returns maximum required number of sensors
	inline static size_t GetMaxNumberOfSensors()
	{
		return ArtificialNeuralNetworkBuilder::GetMaxNumberOfNeuronsPerLayer();
	}

	// Returns number of vehicle inputs
	inline static size_t GetDefaultNumberOfInputs()
	{
		return 3;
	}

	// Returns default sensor's angle offsets
	inline static double GetDefaultSensorAngleOffset()
	{
		return 15.0;
	}

	// Returns default sensor's beam length
	inline static double GetDefaultBeamLength()
	{
		return m_defaultVehicleBeamLength;
	}

	// Returns default sensor's size
	inline static sf::Vector2f GetDefaultSensorSize()
	{
		return m_defaultVehicleSensorSize;
	}

	// Returns maximum vehicle mass
	inline static float GetMaxVehicleMass()
	{
		return m_maxVehicleMass;
	}

	// Slowly calculates vehicle mass based on provided body points
	static float CalculateMass(const std::vector<sf::Vector2f>& vehicleBodyPoints);

	// Returns vehicle prototype
	VehiclePrototype* Get();

	// Checks if dummy can be created
	static bool Initialize();
};