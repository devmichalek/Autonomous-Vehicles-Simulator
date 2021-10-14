#include "VehicleBuilder.hpp"
#include "CoreLogger.hpp"
#include <Box2D/box2d.h>

sf::Vector2f VehicleBuilder::m_maxVehicleBodyBound;
sf::Vector2f VehicleBuilder::m_minVehicleBodyBound;
double VehicleBuilder::m_defaultVehicleBeamLength;
sf::Vector2f VehicleBuilder::m_defaultVehicleSensorSize;
float VehicleBuilder::m_maxVehicleMass;

bool VehicleBuilder::ValidateVehicleBodyNumberOfPoints(size_t count)
{
	if (count < GetMinVehicleBodyNumberOfPoints())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_VEHICLE_BODY_POINTS;
		return false;
	}

	if (count > GetMaxVehicleBodyNumberOfPoints())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_VEHICLE_BODY_POINTS;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateVehicleBodyArea()
{
	auto minBound = GetMinVehicleBodyBound();
	auto minimumArea = minBound.x * minBound.y;
	auto totalArea = DrawableMath::CalculateArea(m_vehiclePrototype.GetBodyPoints());

	if (totalArea < minimumArea)
	{
		m_lastOperationStatus = ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateRelativePositions(const std::vector<sf::Vector2f>& points)
{
	// Find boundaries
	float left = points[0].x;
	float right = points[0].x;
	float top = points[0].y;
	float down = points[0].y;

	for (size_t i = 1; i < points.size(); ++i)
	{
		if (left > points[i].x)
			left = points[i].x;

		if (right < points[i].x)
			right = points[i].x;

		if (top > points[i].y)
			top = points[i].y;

		if (down < points[i].y)
			down = points[i].y;
	}

	float x = float(std::fabs(left - right));
	float y = float(std::fabs(top - down));
	auto maxSize = GetMaxVehicleBodyBound();

	if (x > maxSize.x || y > maxSize.y)
	{
		m_lastOperationStatus = ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateNumberOfSensors(size_t count)
{
	if (count < GetMinNumberOfSensors())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_SENSORS;
		return false;
	}

	if (count > GetMaxNumberOfSensors())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_SENSORS;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateSensorAngle(double angle)
{
	if (size_t(angle) % size_t(GetDefaultSensorAngleOffset()) != 0)
	{
		m_lastOperationStatus = ERROR_SENSOR_ANGLE_IS_NOT_DIVISIBLE;
		return false;
	}

	if (angle < GetMinSensorAngle())
	{
		m_lastOperationStatus = ERROR_SENSOR_ANGLE_IS_TOO_LITTLE;
		return false;
	}

	if (angle > GetMaxSensorAngle())
	{
		m_lastOperationStatus = ERROR_SENSOR_ANGLE_IS_TOO_LARGE;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateSensorMotionRange(double motionRange)
{
	if (motionRange < GetMinSensorMotionRange())
	{
		m_lastOperationStatus = ERROR_SENSOR_MOTION_RANGE_IS_TOO_LITTLE;
		return false;
	}

	if (motionRange > GetMaxSensorMotionRange())
	{
		m_lastOperationStatus = ERROR_SENSOR_MOTION_RANGE_IS_TOO_LARGE;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateSensorPositionsOverVehicleBody()
{
	for (const auto& position : m_vehiclePrototype.GetSensorPoints())
	{
		if (!DrawableMath::IsPointInsidePolygon(m_vehiclePrototype.GetBodyPoints(), position))
		{
			m_lastOperationStatus = ERROR_SENSOR_IS_OUTSIDE_OF_VEHICLE_BODY;
			return false;
		}
	}

	return true;
}

bool VehicleBuilder::ValidateInternal()
{
	if (!ValidateVehicleBodyNumberOfPoints(m_vehiclePrototype.GetNumberOfBodyPoints()))
		return false;

	if (!ValidateVehicleBodyArea())
		return false;

	if (!ValidateRelativePositions(m_vehiclePrototype.GetBodyPoints()))
		return false;

	if (!ValidateNumberOfSensors(m_vehiclePrototype.GetNumberOfSensors()))
		return false;

	if (!ValidateRelativePositions(m_vehiclePrototype.GetSensorPoints()))
		return false;

	if (!ValidateSensorPositionsOverVehicleBody())
		return false;
	
	for (size_t i = 0; i < m_vehiclePrototype.GetNumberOfSensors(); ++i)
	{
		if (!ValidateSensorAngle(m_vehiclePrototype.GetSensorBeamAngle(i)))
			return false;

		if (!ValidateSensorMotionRange(m_vehiclePrototype.GetSensorMotionRange(i)))
			return false;
	}

	return true;
}

void VehicleBuilder::ClearInternal()
{
	m_vehiclePrototype.Clear();
}

bool VehicleBuilder::LoadInternal(std::ifstream& input)
{
	// Get window size
	auto windowSize = CoreWindow::GetSize();
	
	// Read number of vehicle body points
	size_t vehicleBodyNumberOfPoints = 0;
	input.read((char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	if (!ValidateVehicleBodyNumberOfPoints(vehicleBodyNumberOfPoints))
		return false;

	// Read vehicle body points
	for (size_t i = 0; i < vehicleBodyNumberOfPoints; ++i)
	{
		double x = 0.0;
		double y = 0.0;
		input.read((char*)&x, sizeof(x));
		input.read((char*)&y, sizeof(y));
		x *= double(windowSize.x);
		y *= double(windowSize.y);
		m_vehiclePrototype.AddBodyPoint(sf::Vector2f(float(x), float(y)));
	}

	if (!ValidateVehicleBodyArea())
		return false;

	if (!ValidateRelativePositions(m_vehiclePrototype.GetBodyPoints()))
		return false;

	// Read number of vehicle sensors
	size_t numberOfSensors = 0;
	input.read((char*)&numberOfSensors, sizeof(numberOfSensors));

	if (!ValidateNumberOfSensors(numberOfSensors))
		return false;

	// Read vehicle sensor's positions
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double x = 0.0;
		double y = 0.0;
		input.read((char*)&x, sizeof(x));
		input.read((char*)&y, sizeof(y));
		x *= double(windowSize.x);
		y *= double(windowSize.y);
		m_vehiclePrototype.AddSensor(sf::Vector2f(float(x), float(y)), 0.0, GetDefaultSensorMotionRange());
	}

	if (!ValidateRelativePositions(m_vehiclePrototype.GetSensorPoints()))
		return false;

	if (!ValidateSensorPositionsOverVehicleBody())
		return false;

	// Read vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = 0.0;
		input.read((char*)&angle, sizeof(angle));

		if (!ValidateSensorAngle(angle))
			return false;

		m_vehiclePrototype.SetSensorBeamAngle(i, angle);
	}

	// Read vehicle sensor's motion range
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double motionRange = 0.0;
		input.read((char*)&motionRange, sizeof(motionRange));

		if (!ValidateSensorMotionRange(motionRange))
			return false;

		m_vehiclePrototype.SetSensorMotionRange(i, motionRange);
	}

	return true;
}

bool VehicleBuilder::SaveInternal(std::ofstream& output)
{
	// Get window size
	auto windowSize = CoreWindow::GetSize();

	// Save number of vehicle body points
	size_t vehicleBodyNumberOfPoints = m_vehiclePrototype.GetNumberOfBodyPoints();
	output.write((const char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	// Save vehicle body points
	for (size_t i = 0; i < vehicleBodyNumberOfPoints; ++i)
	{
		double x = double(m_vehiclePrototype.GetBodyPoint(i).x) / double(windowSize.x);
		double y = double(m_vehiclePrototype.GetBodyPoint(i).y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save number of vehicle sensors
	size_t numberOfSensors = m_vehiclePrototype.GetNumberOfSensors();
	output.write((const char*)&numberOfSensors, sizeof(numberOfSensors));

	// Save vehicle sensor's positions
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double x = double(m_vehiclePrototype.GetSensorPoint(i).x) / double(windowSize.x);
		double y = double(m_vehiclePrototype.GetSensorPoint(i).y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = m_vehiclePrototype.GetSensorBeamAngle(i);
		output.write((const char*)&angle, sizeof(angle));
	}

	// Save vehicle sensor's motion range
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double motionRange = m_vehiclePrototype.GetSensorMotionRange(i);
		output.write((const char*)&motionRange, sizeof(motionRange));
	}

	return true;
}

void VehicleBuilder::CreateDummyInternal()
{
	sf::Vector2f dummySize = GetMaxVehicleBodyBound();
	dummySize /= 2.0f;

	m_vehiclePrototype.AddBodyPoint(sf::Vector2f(-dummySize.x / 2, -dummySize.y / 2));
	m_vehiclePrototype.AddBodyPoint(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2));
	m_vehiclePrototype.AddBodyPoint(sf::Vector2f(dummySize.x / 2, dummySize.y / 2));
	m_vehiclePrototype.AddBodyPoint(sf::Vector2f(-dummySize.x / 2, dummySize.y / 2));

	m_vehiclePrototype.AddSensor(sf::Vector2f(0, -dummySize.y / 2 + 1), 270.0, GetDefaultSensorMotionRange());
	m_vehiclePrototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, -dummySize.y / 2 + 1), 315.0, GetDefaultSensorMotionRange());
	m_vehiclePrototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, 0), 0, GetDefaultSensorMotionRange());
	m_vehiclePrototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, dummySize.y / 2 - 1), 45.0, GetDefaultSensorMotionRange());
	m_vehiclePrototype.AddSensor(sf::Vector2f(0, dummySize.y / 2 - 1), 90.0, GetDefaultSensorMotionRange());
}

VehicleBuilder::VehicleBuilder() :
	AbstractBuilder()
{
	m_operationsMap[ERROR_TOO_LITTLE_VEHICLE_BODY_POINTS] = "Error: Too little vehicle body points!";
	m_operationsMap[ERROR_TOO_MANY_VEHICLE_BODY_POINTS] = "Error: Too many vehicle body points!";
	m_operationsMap[ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL] = "Error: Vehicle body area is too small!";
	m_operationsMap[ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST] = "Error: Vehicle body boundaries are too vast!";
	m_operationsMap[ERROR_TOO_LITTLE_SENSORS] = "Error: Too little sensors specified!";
	m_operationsMap[ERROR_TOO_MANY_SENSORS] = "Error: Too many sensors specified!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_NOT_DIVISIBLE] = "Error: Sensor's angle must be divisible by 15.0!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_TOO_LITTLE] = "Error: Sensor's angle is too little!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_TOO_LARGE] = "Error: Sensor's angle is too large!";
	m_operationsMap[ERROR_SENSOR_MOTION_RANGE_IS_TOO_LITTLE] = "Error: Sensor's motion range is too little!";
	m_operationsMap[ERROR_SENSOR_MOTION_RANGE_IS_TOO_LARGE] = "Error: Sensor's motion range is too large!";
	m_operationsMap[ERROR_SENSOR_IS_OUTSIDE_OF_VEHICLE_BODY] = "Error: One of sensors is outside of vehicle body!";
	Clear();
}

VehicleBuilder::~VehicleBuilder()
{
}

float VehicleBuilder::CalculateMass(std::vector<sf::Vector2f> vehicleBodyPoints)
{
	// Create shape
	b2World world(b2Vec2(0.f, 0.f));
	const size_t numberOfPoints = vehicleBodyPoints.size();
	b2Vec2* vertices = new b2Vec2[numberOfPoints];
	for (size_t i = 0; i < numberOfPoints; ++i)
		vertices[i] = DrawableMath::ToBox2DPosition(vehicleBodyPoints[i]);
	b2PolygonShape polygonShape;
	polygonShape.Set(vertices, int32(numberOfPoints));
	delete[] vertices;

	// Create fixture definition
	b2FixtureDef fixtureDefinition;
	fixtureDefinition.shape = &polygonShape;
	fixtureDefinition.density = 1.0; // Set max density

	// Create body definition
	b2BodyDef bodyDefinition;
	bodyDefinition.type = b2_dynamicBody;

	// Create body
	b2Body* body = world.CreateBody(&bodyDefinition);
	b2Fixture* fixture = body->CreateFixture(&fixtureDefinition);
	b2MassData massData;
	fixture->GetMassData(&massData);
	return massData.mass;
}

VehiclePrototype* VehicleBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return new VehiclePrototype(m_vehiclePrototype);
}

bool VehicleBuilder::Initialize()
{
	// Initialize static fields
	m_maxVehicleBodyBound = sf::Vector2f(CoreWindow::GetSize().y / 5.0f, CoreWindow::GetSize().x / 15.0f);
	m_minVehicleBodyBound = m_maxVehicleBodyBound / 4.0f;
	m_defaultVehicleBeamLength = double(CoreWindow::GetSize().y) * 0.75;
	m_defaultVehicleSensorSize = sf::Vector2f(CoreWindow::GetSize().x / 400.0f, CoreWindow::GetSize().x / 400.0f);

	// Call internal implementation
	VehicleBuilder builder;
	builder.CreateDummy();

	if (!builder.Validate())
	{
		CoreLogger::PrintError("Cannot create Vehicle Prototype dummy!");
		return false;
	}

	// Initialize max vehicle mass field
	std::vector<sf::Vector2f> bodyPoints;
	sf::Vector2f size = GetMaxVehicleBodyBound();
	bodyPoints.push_back(sf::Vector2f(-size.x / 2, -size.y / 2));
	bodyPoints.push_back(sf::Vector2f(size.x / 2, -size.y / 2));
	bodyPoints.push_back(sf::Vector2f(size.x / 2, size.y / 2));
	bodyPoints.push_back(sf::Vector2f(-size.x / 2, size.y / 2));
	m_maxVehicleMass = CalculateMass(bodyPoints);

	return true;
}
