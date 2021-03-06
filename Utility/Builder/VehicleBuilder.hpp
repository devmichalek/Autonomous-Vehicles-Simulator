#pragma once
#include "AbstractBuilder.hpp"
#include "VehiclePrototype.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "Box2D/b2_shape.h"

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
		ERROR_SENSOR_IS_OUTSIDE_OF_VEHICLE_BODY,
		ERROR_VEHICLE_BODY_IS_NOT_CONVEX_POLYGON
	};

	VehiclePrototype m_prototype;
	static sf::Vector2f m_maxBodyBound;
	static sf::Vector2f m_minBodyBound;
	static double m_defaultBeamLength;
	static sf::Vector2f m_defaultSensorSize;
	static float m_maxMass;
	static float m_maxInertia;

	// Validates number of vehicle body points
	bool ValidateNumberOfBodyPoints(size_t count);

	// Validates vehicle body area
	bool ValidateBodyArea();
	
	// Validate vehicle body edges
	bool ValidateRelativePositions(const std::vector<sf::Vector2f>& points);

	// Validates number of sensors
	bool ValidateNumberOfSensors(size_t count);

	// Validates sensor's angle
	bool ValidateSensorAngle(double angle);

	// Validates sensor's motion range
	bool ValidateSensorMotionRange(double motionRange);

	// Validates sensor's position over vehicle body
	bool ValidateSensorPositionsOverBody();

	// Validates if vehicle body is convex polygon
	bool ValidateBodyAsConvexPolygon();

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

	// Returns mass data of an body consisting of points
	static bool GetMassData(const std::vector<sf::Vector2f>& bodyPoints, b2MassData& massData);
	
public:

	VehicleBuilder();

	~VehicleBuilder();

	// Adds descriptive vehicle body point
	inline void AddBodyPoint(sf::Vector2f point)
	{
		m_prototype.AddBodyPoint(point);
	}

	// Adds vehicle sensor
	inline void AddSensor(sf::Vector2f point, double angle, double motionRange)
	{
		m_prototype.AddSensor(point, angle, motionRange);
	}

	// Returns maximum vehicle body bound
	inline static sf::Vector2f GetMaxBodyBound()
	{
		return m_maxBodyBound;
	}

	// Returns minimum vehicle body bound
	inline static sf::Vector2f GetMinBodyBound()
	{
		return m_minBodyBound;
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

	// Returns default "no torque" value
	inline static double GetDefaultTorque()
	{
		return 0.5;
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
		return m_defaultBeamLength;
	}

	// Returns default sensor's size
	inline static sf::Vector2f GetDefaultSensorSize()
	{
		return m_defaultSensorSize;
	}

	// Returns maximum vehicle mass
	inline static float GetMaxMass()
	{
		return m_maxMass;
	}

	// Returns maximum vehicle inertia
	inline static float GetMaxInertia()
	{
		return m_maxInertia;
	}

	// Returns default color that will be used to color vehicle body based on provided mass
	inline static sf::Color CalculateDefaultColor(const float mass)
	{
		const float ratio = mass / m_maxMass;
		constexpr sf::Uint8 ceiling = sf::Uint8(ColorContext::MaxChannelValue * 0.9f);
		const sf::Uint8 channel = ColorContext::MaxChannelValue - sf::Uint8(ceiling * ratio);
		return ColorContext::Create(channel, channel, channel, ColorContext::MaxChannelValue);
	}

	// Slowly calculates vehicle mass based on provided vehicle body points
	static float CalculateMass(const std::vector<sf::Vector2f>& bodyPoints);

	// Slowly calculates vehicle inertia based on provided vehicle body points
	static float CalculateInertia(const std::vector<sf::Vector2f>& bodyPoints);

	// Returns vehicle prototype
	VehiclePrototype* Get();

	// Checks if dummy can be created
	static bool Initialize();
};