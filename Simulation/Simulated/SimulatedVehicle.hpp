#pragma once
#include "SimulatedAbstract.hpp"
#include "FitnessInterface.hpp"
#include "DrawableVehicle.hpp"
#include "PeriodicTimer.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include <Box2D\Box2D.h>

class SimulatedVehicle final :
	public SimulatedAbstract,
	public FitnessInterface,
	public DrawableVehicle
{
public:

	SimulatedVehicle(const size_t numberOfBodyPoints,
					 const std::vector<sf::Vector2f>& sensorPoints,
					 const std::vector<double>& beamAngles,
					 const std::vector<PeriodicTimer>& motionRanges) :
		SimulatedAbstract(SimulatedAbstract::CategoryVehicle),
		FitnessInterface(),
		DrawableVehicle(numberOfBodyPoints, sensorPoints.size()),
		m_body(nullptr),
		m_numberOfBodyPoints(numberOfBodyPoints),
		m_bodyPoints(nullptr),
		m_sensorPoints(sensorPoints),
		m_beamAngles(beamAngles),
		m_motionRanges(motionRanges),
		m_sensors(sensorPoints.size(), ArtificialNeuralNetworkBuilder::GetMaxNeuronValue()),
		m_active(true)
	{
		// Converts beam deegres angle to radians angles
		for (auto& beamAngle : m_beamAngles)
			beamAngle = DrawableMath::ToRadians(beamAngle);

		// Converts motion ranges deegres values to radians values
		for (auto& motionRange : m_motionRanges)
		{
			motionRange.SetBoundaryValue(DrawableMath::ToRadians(motionRange.GetBoundaryValue()));
			motionRange.SetMultiplier(DrawableMath::ToRadians(motionRange.GetMultiplier()));
			motionRange.SetValue(DrawableMath::ToRadians(motionRange.GetValue()));
		}
	}

	~SimulatedVehicle()
	{
	}

	// Updates friction
	inline void Update(b2World* world)
	{
		// Update body data
		for (size_t i = 0; i < m_numberOfBodyPoints; ++i)
		{
			auto bodyPoint = m_body->GetWorldPoint(m_bodyPoints[i]);
			m_bodyShape.setPoint(i, DrawableMath::ToSFMLPosition(bodyPoint));
		}

		// Update sensor data
		const float radians = m_body->GetAngle();
		const sf::Vector2f center = DrawableMath::ToSFMLPosition(m_body->GetWorldCenter());
		float cosinus = float(cos(radians));
		float sinus = float(sin(radians));
		for (size_t i = 0; i < m_sensorPoints.size(); ++i)
		{
			// Set sensor max value
			m_sensors[i] = ArtificialNeuralNetworkBuilder::GetMaxNeuronValue();

			// Update motion range
			m_motionRanges[i].Update();

			// Calculate sensor position
			m_beams[i][0].position = sf::Vector2f((m_sensorPoints[i].x * cosinus - m_sensorPoints[i].y * sinus),
												  (m_sensorPoints[i].x * sinus + m_sensorPoints[i].y * cosinus));
			m_beams[i][0].position += center;

			// Calculate beam end position
			auto cosBeam = cos(radians + m_beamAngles[i] + m_motionRanges[i].GetValue());
			auto sinBeam = sin(radians + m_beamAngles[i] + m_motionRanges[i].GetValue());
			m_beams[i][1].position.x = static_cast<float>(m_beams[i][0].position.x + VehicleBuilder::GetDefaultBeamLength() * cosBeam);
			m_beams[i][1].position.y = static_cast<float>(m_beams[i][0].position.y + VehicleBuilder::GetDefaultBeamLength() * sinBeam);

			// Raycast
			const b2Vec2 startPoint = DrawableMath::ToBox2DPosition(m_beams[i][0].position);
			const b2Vec2 endPoint = DrawableMath::ToBox2DPosition(m_beams[i][1].position);
			world->RayCast(&m_beamRaycastCallbacks[i], startPoint, endPoint);
		}

		// Lateral linear velocity
		const float elapsedTime = float(CoreWindow::GetElapsedTime());
		b2Vec2 impulse = m_body->GetMass() * -GetLateralVelocity();
		auto impulseLength = impulse.Length();
		if (impulseLength > m_maxLateralImpulse)
		{
			auto factor = m_maxLateralImpulse / impulseLength;
			impulse *= factor;
		}
		impulse *= 35 * elapsedTime;
		m_body->ApplyLinearImpulse(impulse, m_body->GetWorldCenter(), true);

		// Angular velocity
		m_body->ApplyAngularImpulse(10.f * m_body->GetInertia() * elapsedTime * -m_body->GetAngularVelocity(), true);

		// Forward linear velocity
		b2Vec2 currentForwardNormal = GetForwardVelocity();
		float currentForwardSpeed = currentForwardNormal.Normalize();
		float dragForceMagnitude = -2000 * currentForwardSpeed * elapsedTime;
		m_body->ApplyForce(dragForceMagnitude * currentForwardNormal, m_body->GetWorldCenter(), true);
	}

	// Applies forward drive force by specified value (0; 1)
	inline void DriveForward(Neuron value)
	{
		// Find current speed in forward direction
		const b2Vec2 currentForwardNormal = m_body->GetWorldVector(b2Vec2(1, 0));
		const float currentSpeed = b2Dot(GetForwardVelocity(), currentForwardNormal);
		if (currentSpeed < m_maxForwardSpeed)
		{
			// Apply necessary force
			value *= m_maxDriveForce * CoreWindow::GetElapsedTime();
			m_body->ApplyForce(float(value) * currentForwardNormal, m_body->GetWorldCenter(), true);
		}
	}

	// Applies backward drive force by specified value (0; 1)
	inline void DriveBackward(Neuron value)
	{
		// Find current speed in forward direction
		const b2Vec2 currentForwardNormal = m_body->GetWorldVector(b2Vec2(1, 0));
		const float currentSpeed = b2Dot(GetForwardVelocity(), currentForwardNormal);
		if (currentSpeed > m_maxBackwardSpeed)
		{
			// Apply necessary force
			value *= -m_maxDriveForce * CoreWindow::GetElapsedTime();
			m_body->ApplyForce(float(value) * currentForwardNormal, m_body->GetWorldCenter(), true);
		}
	}

	// Applies turn force by specified value (0; 1)
	inline void Turn(Neuron value)
	{
		value *= 2.0;
		value -= 1.0;
		value *= m_maxTorqueForce * CoreWindow::GetElapsedTime();
		m_body->ApplyTorque(float(value), true);
	}

	// Sets body pointer and associated fields
	inline void SetBody(b2Body* body)
	{
		m_body = body;
		auto fixture = body->GetFixtureList();
		auto* shape = (b2PolygonShape*)fixture->GetShape();
		m_numberOfBodyPoints = shape->m_count;
		m_bodyPoints = shape->m_vertices;

		SetDefaultColor(m_body->GetMass());

		// Calculate beam positions
		const float radians = m_body->GetAngle();
		const sf::Vector2f center = DrawableMath::ToSFMLPosition(m_body->GetWorldCenter());
		float cosinus = float(cos(radians));
		float sinus = float(sin(radians));
		for (size_t i = 0; i < m_sensorPoints.size(); ++i)
		{
			m_beams[i][0].position = sf::Vector2f((m_sensorPoints[i].x * cosinus - m_sensorPoints[i].y * sinus),
												  (m_sensorPoints[i].x * sinus + m_sensorPoints[i].y * cosinus));
			m_beams[i][0].position += center;

			// Calculate beam end position
			auto cosBeam = cos(radians + m_beamAngles[i] + m_motionRanges[i].GetValue());
			auto sinBeam = sin(radians + m_beamAngles[i] + m_motionRanges[i].GetValue());
			m_beams[i][1].position.x = static_cast<float>(m_beams[i][0].position.x + VehicleBuilder::GetDefaultBeamLength() * cosBeam);
			m_beams[i][1].position.y = static_cast<float>(m_beams[i][0].position.y + VehicleBuilder::GetDefaultBeamLength() * sinBeam);
			m_beamRaycastCallbacks.emplace_back(m_beams[i], m_sensors[i]);
		}
			
		m_beamRaycastCallbacks.shrink_to_fit();
	}

	// Returns true if vehicle is active
	inline bool IsActive()
	{
		return m_active;
	}

	// Makes vehicle inactive
	inline void SetInactive()
	{
		m_active = false;
		for (auto& beam : m_beams)
			beam[0].color = beam[1].color = sf::Color(255, 255, 255, 0);
	}

	// Returns vehicle center
	inline sf::Vector2f GetCenter()
	{
		return DrawableMath::ToSFMLPosition(m_body->GetWorldCenter());
	}

	// Output data
	inline const NeuronLayer& ProcessOutput() const
	{
		return m_sensors;
	}

	// Input data
	inline void ProcessInput(const NeuronLayer& layer)
	{
		DriveForward(layer[0]);
		DriveBackward(layer[1]);
		Turn(layer[2]);
	}

private:

	// Calculates lateral velocity
	b2Vec2 GetLateralVelocity()
	{
		b2Vec2 currentRightNormal = m_body->GetWorldVector(b2Vec2(0, 1));
		return b2Dot(currentRightNormal, m_body->GetLinearVelocity()) * currentRightNormal;
	}

	// Calculates forward velocitys
	b2Vec2 GetForwardVelocity()
	{
		b2Vec2 currentForwardNormal = m_body->GetWorldVector(b2Vec2(1, 0));
		return b2Dot(currentForwardNormal, m_body->GetLinearVelocity()) * currentForwardNormal;
	}

	// Body data
	b2Body* m_body;
	size_t m_numberOfBodyPoints;
	b2Vec2* m_bodyPoints;

	// Sensors data
	std::vector<sf::Vector2f> m_sensorPoints;
	std::vector<double> m_beamAngles;
	std::vector<PeriodicTimer> m_motionRanges;
	NeuronLayer m_sensors;

	// Features
	inline static const double m_maxForwardSpeed = 20.0;
	inline static const double m_maxBackwardSpeed = -12.0;
	inline static const double m_maxDriveForce = 50000.0;
	inline static const double m_maxTorqueForce = 600000.0;
	inline static const float m_maxLateralImpulse = 2.5f;
	bool m_active;

	// Raycast callback
	class BeamRaycastCallback :
		public b2RayCastCallback
	{
		EdgeShape& m_beam;
		Neuron& m_sensor;

	public:

		BeamRaycastCallback(EdgeShape& beam, Neuron& sensor) :
			m_beam(beam),
			m_sensor(sensor)
		{
		}
		
		float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
		{
			//switch (fixture->GetFilterData().categoryBits)
			//{
			//	case SimulatedAbstract::CategoryEdge:
					m_beam[1].position = DrawableMath::ToSFMLPosition(point);
					m_sensor = Neuron(fraction);
					return fraction;
			//	case SimulatedAbstract::CategoryCheckpoint:
			//	case SimulatedAbstract::CategoryVehicle:
			//	default:
			//		break;
			//}

			// Don't clip the ray and continue
			//return 1;
		}
	};

	std::vector<BeamRaycastCallback> m_beamRaycastCallbacks;
};

using SimulatedVehicles = std::vector<SimulatedVehicle*>;