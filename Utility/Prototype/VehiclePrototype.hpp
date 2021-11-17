#pragma once
#include "MathContext.hpp"
#include "PeriodicTimer.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class VehiclePrototype final
{
	// Body data
	sf::Vector2f m_center;
	double m_angle;
	std::vector<sf::Vector2f> m_bodyPoints;

	// Sensors data
	EdgeVector m_beamVector;
	std::vector<sf::Vector2f> m_sensorPoints;
	std::vector<double> m_beamAngles;
	std::vector<PeriodicTimer> m_motionRanges;
	
	// Shapes
	sf::ConvexShape m_bodyShape;
	EdgeShape m_beamShape;
	sf::CircleShape m_sensorShape;

public:

	VehiclePrototype();

	~VehiclePrototype();

	// Clears vectors
	void Clear();

	// Sets vehicle center position
	inline void SetCenter(sf::Vector2f center)
	{
		m_center = center;
	}

	// Returns vehicle center position
	inline const sf::Vector2f& GetCenter() const
	{
		return m_center;
	}

	// Sets vehicle angle
	inline void SetAngle(double angle)
	{
		m_angle = angle;
	}

	// Returns vehicle angle
	inline double GetAngle() const
	{
		return m_angle;
	}

	// Adds new  body descriptive point relative to the center, returns true in case of success
	bool AddBodyPoint(sf::Vector2f point);

	// Removes last point describing body, returns true if any point was removed
	bool RemoveLastBodyPoint();

	// Returns body point
	sf::Vector2f GetBodyPoint(size_t index) const;

	// Returns points describing body
	inline const std::vector<sf::Vector2f>& GetBodyPoints() const
	{
		return m_bodyPoints;
	}

	// Returns number of points describing vehicle body
	inline size_t GetNumberOfBodyPoints() const
	{
		return m_bodyPoints.size();
	}

	// Returns number of sensors
	inline size_t GetNumberOfSensors() const
	{
		return m_beamVector.size();
	}

	// Change sensor's beam angle
	void SetSensorBeamAngle(size_t index, double angle);

	// Returns sensor's beam angle
	double GetSensorBeamAngle(size_t index) const;

	// Returns sensors beam angles
	const std::vector<double> GetSensorBeamAngles() const
	{
		return m_beamAngles;
	}

	// Adds new sensor
	void AddSensor(sf::Vector2f point, double angle, double motionRange);

	// Removes particular sensor
	void RemoveSensor(size_t index);

	// Checks if there is intersection if yes then returns proper sensor index
	bool GetSensorIndex(size_t& index, sf::Vector2f point) const;

	// Returns sensors points
	const std::vector<sf::Vector2f>& GetSensorPoints()
	{
		return m_sensorPoints;
	}

	// Returns specific sensor point
	sf::Vector2f GetSensorPoint(size_t index);

	// Change sensor's motion range
	void SetSensorMotionRange(size_t index, double motionRange);

	// Returns sensor's motion range
	double GetSensorMotionRange(size_t index) const;

	// Returns sensor's motion range
	const std::vector<PeriodicTimer>& GetSensorMotionRanges() const
	{
		return m_motionRanges;
	}

	// Generates sensor's motion range start value
	double GenerateMotionRangeValue(double motionRange) const;

	// Update vehicle rotation and position
	void Update();

	// Draws vehicle body
	void DrawBody();

	// Draws vehicle sensors
	void DrawSensors();

	// Draws vehicle sensors beams
	void DrawBeams();

	// Draws marked sensor shape
	void DrawMarkedSensor(size_t index);
};

using VehiclePrototypes = std::vector<VehiclePrototype*>;
