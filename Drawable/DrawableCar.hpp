#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include "CoreWindow.hpp"
#include "DrawableMath.hpp"
#include "Neural.hpp"

class DrawableCar
{
	// Car attributes
	double m_angle; // Angle in relation to x axis
	double m_speed;
	inline static const double m_rotationConst = 150.0;
	inline static const double m_maxSpeed = 1.5;
	inline static const double m_minSpeed = 0.0;
	inline static const double m_speedFactor = 300.0;

	// Car data
	using Body = std::array<sf::Vertex, CAR_NUMBER_OF_POINTS>;
	Body m_body;
	sf::Vector2f m_center;
	sf::Vector2f m_size;
	CarPoints m_points;

	// Beam data
	using Beams = std::vector<Edge>;
	using BeamAngles = std::vector<double>;
	using BeamStartPosition = std::tuple<size_t, size_t, double>; // Start index, end index, percentage of distance from start index
	using BeamStartPositions = std::vector<BeamStartPosition>;
	double m_beamReach;
	Line m_beamShape;
	Beams m_beams;
	BeamAngles m_beamAngles;
	BeamStartPositions m_beamStartPositions;

	// Sensor data
	sf::Vector2f m_sensorSize;
	sf::CircleShape m_sensorShape;
	NeuronLayer m_sensors;
	inline static const Neuron m_sensorMaxValue = 1.0;

public:
	DrawableCar() :
		m_angle(0.0),
		m_speed(0.0),
		m_center(sf::Vector2f(0.0f, 0.0f))
	{
		auto windowSize = CoreWindow::GetSize();
		const float widthFactor = 30.0f;
		const float heightFactor = 10.0f;
		m_size = sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);
		setCenter(sf::Vector2f(0.0f, 0.0f));
		m_beamReach = double(CoreWindow::GetSize().y) * 0.75;
		m_beamShape[0].color = sf::Color(255, 255, 255, 144);
		m_beamShape[1].color = sf::Color(255, 255, 255, 32);
		m_sensorSize = sf::Vector2f(m_size.x / widthFactor, m_size.x / widthFactor);
		m_sensorShape.setRadius(m_sensorSize.x);
		m_sensorShape.setFillColor(sf::Color::Red);
		setFollowerColor();
		init();
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

	// Prepare sensors and beams
	void init(size_t numberOfSensors = CAR_DEFAULT_NUMBER_OF_SENSORS);

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
	void processInput(const NeuronLayer& layer)
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
		sf::Vector2f intersectionPoint;
		for (size_t i = 0; i < m_beams.size(); ++i)
		{
			if (GetIntersectionPoint(edge, m_beams[i], intersectionPoint))
			{
				m_beams[i][1] = intersectionPoint;
				m_sensors[i] = Distance(m_beams[i]) / m_beamReach;
			}
		}
	}
};
