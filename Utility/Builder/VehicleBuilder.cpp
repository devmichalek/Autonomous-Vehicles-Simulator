#include "VehicleBuilder.hpp"
#include "CoreLogger.hpp"
#include <Box2D/box2d.h>

sf::Vector2f VehicleBuilder::m_maxBodyBound;
sf::Vector2f VehicleBuilder::m_minBodyBound;
double VehicleBuilder::m_defaultBeamLength;
sf::Vector2f VehicleBuilder::m_defaultSensorSize;
float VehicleBuilder::m_maxMass;

bool VehicleBuilder::ValidateNumberOfBodyPoints(size_t count)
{
	if (count < GetMinNumberOfBodyPoints())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_VEHICLE_BODY_POINTS;
		return false;
	}

	if (count > GetMaxNumberOfBodyPoints())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_VEHICLE_BODY_POINTS;
		return false;
	}

	return true;
}

bool VehicleBuilder::ValidateBodyArea()
{
	auto minBound = GetMinBodyBound();
	auto minimumArea = minBound.x * minBound.y;
	auto totalArea = DrawableMath::CalculateArea(m_prototype.GetBodyPoints());

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
	auto maxSize = GetMaxBodyBound();

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

bool VehicleBuilder::ValidateSensorPositionsOverBody()
{
	for (const auto& position : m_prototype.GetSensorPoints())
	{
		if (!DrawableMath::IsPointInsidePolygon(m_prototype.GetBodyPoints(), position))
		{
			m_lastOperationStatus = ERROR_SENSOR_IS_OUTSIDE_OF_VEHICLE_BODY;
			return false;
		}
	}

	return true;
}

bool VehicleBuilder::ValidateInternal()
{
	if (!ValidateNumberOfBodyPoints(m_prototype.GetNumberOfBodyPoints()))
		return false;

	if (!ValidateBodyArea())
		return false;

	if (!ValidateRelativePositions(m_prototype.GetBodyPoints()))
		return false;

	if (!ValidateNumberOfSensors(m_prototype.GetNumberOfSensors()))
		return false;

	if (!ValidateRelativePositions(m_prototype.GetSensorPoints()))
		return false;

	if (!ValidateSensorPositionsOverBody())
		return false;
	
	for (size_t i = 0; i < m_prototype.GetNumberOfSensors(); ++i)
	{
		if (!ValidateSensorAngle(m_prototype.GetSensorBeamAngle(i)))
			return false;

		if (!ValidateSensorMotionRange(m_prototype.GetSensorMotionRange(i)))
			return false;
	}

	return true;
}

void VehicleBuilder::ClearInternal()
{
	m_prototype.Clear();
}

bool VehicleBuilder::LoadInternal(std::ifstream& input)
{
	// Get window size
	auto windowSize = CoreWindow::GetWindowSize();
	
	// Read number of vehicle body points
	size_t vehicleBodyNumberOfPoints = 0;
	input.read((char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	if (!ValidateNumberOfBodyPoints(vehicleBodyNumberOfPoints))
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
		m_prototype.AddBodyPoint(sf::Vector2f(float(x), float(y)));
	}

	if (!ValidateBodyArea())
		return false;

	if (!ValidateRelativePositions(m_prototype.GetBodyPoints()))
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
		m_prototype.AddSensor(sf::Vector2f(float(x), float(y)), 0.0, GetDefaultSensorMotionRange());
	}

	if (!ValidateRelativePositions(m_prototype.GetSensorPoints()))
		return false;

	if (!ValidateSensorPositionsOverBody())
		return false;

	// Read vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = 0.0;
		input.read((char*)&angle, sizeof(angle));

		if (!ValidateSensorAngle(angle))
			return false;

		m_prototype.SetSensorBeamAngle(i, angle);
	}

	// Read vehicle sensor's motion range
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double motionRange = 0.0;
		input.read((char*)&motionRange, sizeof(motionRange));

		if (!ValidateSensorMotionRange(motionRange))
			return false;

		m_prototype.SetSensorMotionRange(i, motionRange);
	}

	return true;
}

bool VehicleBuilder::SaveInternal(std::ofstream& output)
{
	// Get window size
	auto windowSize = CoreWindow::GetWindowSize();

	// Save number of vehicle body points
	size_t vehicleBodyNumberOfPoints = m_prototype.GetNumberOfBodyPoints();
	output.write((const char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	// Save vehicle body points
	for (size_t i = 0; i < vehicleBodyNumberOfPoints; ++i)
	{
		double x = double(m_prototype.GetBodyPoint(i).x) / double(windowSize.x);
		double y = double(m_prototype.GetBodyPoint(i).y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save number of vehicle sensors
	size_t numberOfSensors = m_prototype.GetNumberOfSensors();
	output.write((const char*)&numberOfSensors, sizeof(numberOfSensors));

	// Save vehicle sensor's positions
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double x = double(m_prototype.GetSensorPoint(i).x) / double(windowSize.x);
		double y = double(m_prototype.GetSensorPoint(i).y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = m_prototype.GetSensorBeamAngle(i);
		output.write((const char*)&angle, sizeof(angle));
	}

	// Save vehicle sensor's motion range
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double motionRange = m_prototype.GetSensorMotionRange(i);
		output.write((const char*)&motionRange, sizeof(motionRange));
	}

	return true;
}

void VehicleBuilder::CreateDummyInternal()
{
	sf::Vector2f dummySize = GetMaxBodyBound();
	dummySize /= 2.0f;

	m_prototype.AddBodyPoint(sf::Vector2f(-dummySize.x / 2, -dummySize.y / 2));
	m_prototype.AddBodyPoint(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2));
	m_prototype.AddBodyPoint(sf::Vector2f(dummySize.x / 2, dummySize.y / 2));
	m_prototype.AddBodyPoint(sf::Vector2f(-dummySize.x / 2, dummySize.y / 2));

	m_prototype.AddSensor(sf::Vector2f(0, -dummySize.y / 2 + 1), 270.0, GetDefaultSensorMotionRange());
	m_prototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, -dummySize.y / 2 + 1), 315.0, GetDefaultSensorMotionRange());
	m_prototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, 0), 0, GetDefaultSensorMotionRange());
	m_prototype.AddSensor(sf::Vector2f(dummySize.x / 2 - 1, dummySize.y / 2 - 1), 45.0, GetDefaultSensorMotionRange());
	m_prototype.AddSensor(sf::Vector2f(0, dummySize.y / 2 - 1), 90.0, GetDefaultSensorMotionRange());
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

float VehicleBuilder::CalculateMass(const std::vector<sf::Vector2f>& bodyPoints)
{
	if (bodyPoints.size() < GetMinNumberOfBodyPoints())
		return 0.f;

	// Create shape
	b2World world(b2Vec2(0.f, 0.f));
	const size_t numberOfPoints = bodyPoints.size();
	b2Vec2* vertices = new b2Vec2[numberOfPoints];
	for (size_t i = 0; i < numberOfPoints; ++i)
		vertices[i] = DrawableMath::ToBox2DPosition(bodyPoints[i]);
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

	return new VehiclePrototype(m_prototype);
}

bool VehicleBuilder::Initialize()
{
	// Initialize static fields
	m_maxBodyBound = sf::Vector2f(CoreWindow::GetWindowSize().y / 5.0f, CoreWindow::GetWindowSize().x / 15.0f);
	m_minBodyBound = m_maxBodyBound / 4.0f;
	m_defaultBeamLength = double(CoreWindow::GetWindowSize().y) * 0.75;
	m_defaultSensorSize = sf::Vector2f(CoreWindow::GetWindowSize().x / 400.0f, CoreWindow::GetWindowSize().x / 400.0f);

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
	sf::Vector2f size = GetMaxBodyBound();
	bodyPoints.push_back(sf::Vector2f(-size.x / 2, -size.y / 2));
	bodyPoints.push_back(sf::Vector2f(size.x / 2, -size.y / 2));
	bodyPoints.push_back(sf::Vector2f(size.x / 2, size.y / 2));
	bodyPoints.push_back(sf::Vector2f(-size.x / 2, size.y / 2));
	m_maxMass = CalculateMass(bodyPoints);

	return true;
}
