#pragma once
#include "VehicleBody.hpp"
#include "VehicleSensors.hpp"

class DrawableVehicleBuilder;

class DrawableVehicle final
{
	inline static const double m_rotationConst = 150.0;
	inline static const double m_maxSpeed = 1.5;
	inline static const double m_minSpeed = 0.0;
	inline static const double m_speedFactor = 300.0;

	bool m_active;
	double m_angle;
	double m_sinus;
	double m_cosinus;
	double m_speed;
	sf::Vector2f m_center;
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	friend DrawableVehicleBuilder;
	
	DrawableVehicle(VehicleBody vehicleBody, VehicleSensors vehicleSensors);

public:

	~DrawableVehicle();

	// Returns true if vehicle is active
	inline bool IsActive()
	{
		return m_active;
	}

	// Makes vehicle inactive
	inline void SetInactive()
	{
		m_active = false;
	}

	// Makes vehicle active
	inline void SetActive()
	{
		m_active = true;
	}

	// Initializes static fields
	static void Initialize();

	// Set this vehicle as a leading one
	inline void SetLeaderColor()
	{
		m_vehicleBody.SetLeaderColor();
	}

	// Set this vehicle as a following one
	inline void SetFollowerColor()
	{
		m_vehicleBody.SetFollowerColor();
	}

	// Sets center position
	inline void SetCenter(sf::Vector2f center)
	{
		m_center = center;
	}

	// Returns vehicle center
	inline const sf::Vector2f& GetCenter()
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

	// Returns true if given point is inside vehicle area
	inline bool Inside(sf::Vector2f point)
	{
		return m_vehicleBody.Inside(point);
	}

	// Rotate vehicle by specified value (0; 1)
	void Rotate(Neuron value);

	// Accelerate by specified value (0; 1)
	void Accelerate(Neuron value);

	// Brake by specified value (0; 1)
	void Brake(Neuron value);
	
	// Update vehicle rotation and position
	void Update();

	// Draws vehicle body
	void DrawBody();

	// Draws vehicle sensor beams
	void DrawBeams();

	// Output sensors data
	inline const NeuronLayer& ProcessOutput()
	{
		return m_vehicleSensors.GetSensorsReading();
	}

	// Input data
	inline void ProcessInput(const NeuronLayer& layer)
	{
		Accelerate(layer[0]);
		Rotate(layer[1]);
		Brake(layer[2]);
	}

	inline static size_t GetNumberOfInputs()
	{
		// Accelerate(), Rotate(), Brake()
		return 3;
	}

	// Return vehicle described in vertices
	inline const sf::VertexArray& GetVertices()
	{
		return m_vehicleBody.GetVertices();
	}

	// Update sensors and its beams
	inline void Detect(const Edge& edge)
	{
		m_vehicleSensors.Detect(edge);
	}
};

using DrawableVehicleFactory = std::vector<DrawableVehicle*>;
