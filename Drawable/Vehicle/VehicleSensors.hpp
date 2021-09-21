#pragma once
#include "DrawableMath.hpp"
#include "Neural.hpp"
#include "CoreWindow.hpp"
#include "PeriodicTimer.hpp"
#include <random>
#include <SFML/Graphics/CircleShape.hpp>

class VehicleSensors final
{
	VehicleSensors();

	~VehicleSensors();

	// Initializes static fields
	static void Initialize();

	// Clears dynamically allocated memory
	void Clear();

	// Set up base components
	void SetBase(const sf::Vector2f* center,
				 const double* angle,
				 const double* sinus,
				 const double* cosinus);

	// Updates each sensor position
	void Update();

	// Draws sensor beams
	void DrawBeams();

	// Draws sensors
	void DrawSensors();

	// Calculates each beam length
	inline void Detect(const Edge& edge)
	{
		sf::Vector2f intersectionPoint;
		for (size_t i = 0; i < m_beamVector.size(); ++i)
		{
			if (DrawableMath::GetIntersectionPoint(edge, m_beamVector[i], intersectionPoint))
			{
				m_beamVector[i][1] = intersectionPoint;
				m_sensors[i] = DrawableMath::Distance(m_beamVector[i]) / m_beamLength;
			}
		}
	}

	// Change sensor's beam angle
	void SetSensorAngle(size_t index, double angle);

	// Returns sensor's beam angle
	double GetSensorAngle(size_t index) const;

	// Change sensor's motion range
	void SetSensorMotionRange(size_t index, double motionRange);

	// Returns sensor's motion range
	double GetSensorMotionRange(size_t index) const;

	// Generates sensor's motion range start value
	double GenerateMotionRangeValue(double motionRange) const;

	// Adds new sensor
	void AddSensor(sf::Vector2f point, double angle, double motionRange);

	// Removes particular sensor
	void RemoveSensor(size_t index);

	// Checks if there is intersection if yes then returns proper sensor index
	bool GetSensorIndex(size_t& index, sf::Vector2f point) const;

	// Returns number of sensors
	inline size_t GetNumberOfSensors() const
	{
		return m_sensors.size();
	}

	// Returns sensors readings
	inline const NeuronLayer& GetSensorsReading() const
	{
		return m_sensors;
	}

	using BeamVector = EdgeVector;
	using AngleVector = std::vector<double>;

	// Friend classes
	friend class DrawableVehicle;
	friend class DrawableVehicleBuilder;
	friend class StateVehicleEditor;

	BeamVector m_beamVector;
	std::vector<sf::Vector2f> m_points;
	AngleVector m_angleVector;
	NeuronLayer m_sensors;
	std::vector<PeriodicTimer> m_motionRanges;

	const sf::Vector2f* m_center;
	const double* m_angle;
	const double* m_sinus;
	const double* m_cosinus;

	static double m_beamLength;
	static sf::Vector2f m_sensorSize;
	static sf::Vector2f m_baseCenter;
	static double m_baseAngle;
	static double m_baseSinus;
	static double m_baseCosinus;
	static Line m_beamShape;
	static sf::CircleShape m_sensorShape;
	inline static const Neuron m_sensorMaxValue = 1.0;
	inline static const Neuron m_sensorMinValue = 0.0;
	static std::mt19937 m_mersenneTwister;
	static bool m_initialized;
};