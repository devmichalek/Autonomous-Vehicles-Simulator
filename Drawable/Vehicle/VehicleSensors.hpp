#pragma once
#include "DrawableMath.hpp"
#include "Neural.hpp"
#include "CoreWindow.hpp"
#include <SFML/Graphics/CircleShape.hpp>

class DrawableVehicle;
class DrawableVehicleBuilder;
class StateVehicleEditor;

class VehicleSensors final
{
	using BeamVector = EdgeVector;
	using OffsetVector = std::vector<sf::Vector2f>;
	using AngleVector = std::vector<double>;
	friend DrawableVehicle;
	friend DrawableVehicleBuilder;
	friend StateVehicleEditor;

	BeamVector m_beamVector;
	OffsetVector m_offsetVector;
	AngleVector m_angleVector;
	NeuronLayer m_sensors;

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
	static bool m_initialized;

	VehicleSensors();

	~VehicleSensors();

	// Initializes static fields
	static void Initialize();

	// Clears dynamicly allocated memory
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

	// Adds new sensor
	void AddSensor(sf::Vector2f offset, double angle);

	// Removes sensor if given point intersects with particular sensor
	void RemoveSensor(sf::Vector2f point);

	// Returns sensors readings
	inline const NeuronLayer& GetSensorsReading()
	{
		return m_sensors;
	}
};