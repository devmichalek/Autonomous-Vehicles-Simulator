#pragma once
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "CoreWindow.hpp"
#include "DrawableMath.hpp"
#include "Neural.hpp"

class DrawableCar
{
	// Car attributes
	double m_angle;
	double m_speed;
	inline static const double m_rotationConst = 150.0;
	inline static const double m_maxSpeedConst = 1.5;
	inline static const double m_minSpeedConst = 0;
	inline static const double m_speedConst = 300.0;

	// Car data
	sf::ConvexShape m_carShape;
	sf::Vector2f m_size;
	sf::Vector2f m_center;
	CarPoints m_points;

	// Beam data
	Line m_beamShape;
	const double m_beamReach;
	CarBeams m_beams;
	CarBeamAngles m_beamAngles;

	// Sensor data
	sf::CircleShape m_sensorShape;
	sf::Vector2f m_sensorSize;
	NeuronLayer m_sensors;
	inline static const Neuron m_sensorMaxValue = 1.0;

public:
	DrawableCar() :
		m_angle(0.0),
		m_speed(0.0),
		m_center(0.0f, 0.0f),
		m_beamReach(double(CoreWindow::getSize().y) * 0.75),
		m_beamAngles({270.0, 315.0, 0.0, 45.0, 90.0})
	{
		auto windowSize = CoreWindow::getSize();
		const float widthFactor = 30.0f;
		const float heightFactor = 10.0f;
		m_size = sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);
		m_carShape.setPointCount(CAR_NUMBER_OF_POINTS);
		m_sensorSize = sf::Vector2f(m_size.x / widthFactor, m_size.x / widthFactor);
		m_sensorShape.setRadius(m_sensorSize.x);
		m_sensorShape.setFillColor(sf::Color::Red);
		m_beamShape[0].color = sf::Color(255, 255, 255, 144);
		m_beamShape[1].color = sf::Color(255, 255, 255, 32);
		m_sensors.resize(CAR_NUMBER_OF_SENSORS);
		update();
	}

	virtual ~DrawableCar()
	{
	}

	// Set this car as a leading one
	void setLeaderColor();

	// Set this car as a following one
	void setFollowerColor();

	// Sets center position
	void setCenter(const sf::Vector2f center);

	// Returns car center
	sf::Vector2f getCenter();

	// Sets car angle
	void setAngle(double angle);

	// Returns car angle
	double getAngle() const;

	// Returns true if given point is inside car rectangle
	bool inside(sf::Vector2f point);

	// Rotate car by specified value (0; 1)
	void rotate(Neuron value);

	// Accelerate by specified value (0; 1)
	void accelerate(Neuron value);

	// Brake by specified value (0; 1)
	void brake(Neuron value);
	
	// Update car rotation and position
	void update();

	// Draws car body
	void drawBody();

	// Draws car sensor beams
	void drawBeams();

	// Output sensors data
	NeuronLayer processOutput()
	{
		return m_sensors;
	}

	// Input data
	void processInput(NeuronLayer& layer)
	{
		accelerate(layer[0]);
		rotate(layer[1]);
		brake(layer[2]);
	}

	// Return car described in points
	const CarPoints& getPoints()
	{
		return m_points;
	}

	// Update sensors and its beams
	inline void detect(const Edge& edge)
	{
		sf::Vector2f ipoint; // Intersection point
		for (size_t i = 0; i < CAR_NUMBER_OF_SENSORS; ++i)
		{
			if (GetIntersectionPoint(edge, m_beams[i], ipoint))
			{
				m_beams[i][1] = ipoint;
				m_sensors[i] = Distance(m_beams[i]) / m_beamReach;
			}
		}
	}
};
