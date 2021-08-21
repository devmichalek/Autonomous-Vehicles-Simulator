#include "DrawableVehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"

bool DrawableVehicleBuilder::ValidateVehicleBodyNumberOfEdges(size_t count)
{
	if (count < GetMinVehicleBodyNumberOfEdges())
	{
		m_lastOperationStatus = ERROR_TOO_LITTLE_VEHICLE_BODY_EDGES;
		return false;
	}

	if (count > GetMaxVehicleBodyNumberOfEdges())
	{
		m_lastOperationStatus = ERROR_TOO_MANY_VEHICLE_BODY_EDGES;
		return false;
	}

	return true;
}

double DrawableVehicleBuilder::CalculateVehicleBodyArea(std::vector<sf::Vector2f> points)
{
	double area = 0.0;
	for (size_t i = 2; i < points.size(); ++i)
		area += DrawableMath::GetTriangleArea(points[i - 2], points[i - 1], points[i]);
	return area;
}

bool DrawableVehicleBuilder::ValidateVehicleBodyArea()
{
	sf::Vector2f dummySize = GetMaxVehicleBodySize() / 4.0f;
	auto dummy = CreateVehicleBodyDummy(dummySize);
	double minimumArea = CalculateVehicleBodyArea(dummy.m_points);
	double totalArea = CalculateVehicleBodyArea(m_vehicleBody.m_points);

	if (totalArea < minimumArea)
	{
		m_lastOperationStatus = ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL;
		return false;
	}

	return true;
}

bool DrawableVehicleBuilder::ValidateRelativePositions(const std::vector<sf::Vector2f>& points)
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
	auto maxSize = GetMaxVehicleBodySize();

	if (x > maxSize.x || y > maxSize.y)
	{
		m_lastOperationStatus = ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST;
		return false;
	}

	return true;
}

bool DrawableVehicleBuilder::ValidateNumberOfSensors(size_t count)
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

bool DrawableVehicleBuilder::ValidateSensorAngle(double angle)
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

bool DrawableVehicleBuilder::ValidateInternal()
{
	if (!ValidateVehicleBodyNumberOfEdges(m_vehicleBody.GetNumberOfPoints()))
		return false;

	if (!ValidateVehicleBodyArea())
		return false;

	if (!ValidateRelativePositions(m_vehicleBody.m_points))
		return false;

	if (!ValidateNumberOfSensors(m_vehicleSensors.GetNumberOfSensors()))
		return false;

	if (!ValidateRelativePositions(m_vehicleSensors.m_offsetVector))
		return false;

	for (const auto& angle : m_vehicleSensors.m_angleVector)
	{
		if (!ValidateSensorAngle(angle))
			return false;
	}

	return true;
}

void DrawableVehicleBuilder::ClearInternal()
{
	m_vehicleBody.Clear();
	m_vehicleSensors.Clear();
}

bool DrawableVehicleBuilder::LoadInternal(std::ifstream& input)
{
	// Get window size
	auto windowSize = CoreWindow::GetSize();
	
	// Read number of vehicle body points
	size_t vehicleBodyNumberOfPoints = 0;
	input.read((char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	if (!ValidateVehicleBodyNumberOfEdges(vehicleBodyNumberOfPoints))
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
		m_vehicleBody.AddPoint(sf::Vector2f(float(x), float(y)));
	}

	if (!ValidateVehicleBodyArea())
		return false;

	if (!ValidateRelativePositions(m_vehicleBody.m_points))
		return false;

	// Read number of vehicle sensors
	size_t numberOfSensors = 0;
	input.read((char*)&numberOfSensors, sizeof(numberOfSensors));

	if (!ValidateNumberOfSensors(numberOfSensors))
		return false;

	// Read vehicle sensor's positions
	for (size_t i = 0; i < vehicleBodyNumberOfPoints; ++i)
	{
		double x = 0.0;
		double y = 0.0;
		input.read((char*)&x, sizeof(x));
		input.read((char*)&y, sizeof(y));
		x *= double(windowSize.x);
		y *= double(windowSize.y);
		m_vehicleSensors.AddSensor(sf::Vector2f(float(x), float(y)), 0.0);
	}

	if (!ValidateRelativePositions(m_vehicleSensors.m_offsetVector))
		return false;

	// Read vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = 0.0;
		input.read((char*)&angle, sizeof(angle));

		if (!ValidateSensorAngle(angle))
			return false;
	}

	return true;
}

bool DrawableVehicleBuilder::SaveInternal(std::ofstream& output)
{
	// Get window size
	auto windowSize = CoreWindow::GetSize();

	// Save number of vehicle body points
	size_t vehicleBodyNumberOfPoints = m_vehicleBody.GetNumberOfPoints();
	output.write((const char*)&vehicleBodyNumberOfPoints, sizeof(vehicleBodyNumberOfPoints));

	// Save vehicle body points
	for (size_t i = 0; i < vehicleBodyNumberOfPoints; ++i)
	{
		double x = double(m_vehicleBody.m_points[i].x) / double(windowSize.x);
		double y = double(m_vehicleBody.m_points[i].y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save number of vehicle sensors
	size_t numberOfSensors = m_vehicleSensors.GetNumberOfSensors();
	output.write((const char*)&numberOfSensors, sizeof(numberOfSensors));

	// Save vehicle sensor's positions
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double x = double(m_vehicleSensors.m_offsetVector[i].x) / double(windowSize.x);
		double y = double(m_vehicleSensors.m_offsetVector[i].y) / double(windowSize.y);
		output.write((const char*)&x, sizeof(x));
		output.write((const char*)&y, sizeof(y));
	}

	// Save vehicle sensor's angles
	for (size_t i = 0; i < numberOfSensors; ++i)
	{
		double angle = m_vehicleSensors.m_angleVector[i];
		output.write((const char*)&angle, sizeof(angle));
	}

	return true;
}

VehicleBody DrawableVehicleBuilder::CreateVehicleBodyDummy(sf::Vector2f dummySize)
{
	VehicleBody dummy;
	dummy.AddPoint(sf::Vector2f(-dummySize.x / 2, -dummySize.y / 2));
	dummy.AddPoint(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2));
	dummy.AddPoint(sf::Vector2f(-dummySize.x / 2, dummySize.y / 2));
	dummy.AddPoint(sf::Vector2f(dummySize.x / 2, dummySize.y / 2));
	return dummy;
}

DrawableVehicleBuilder::DrawableVehicleBuilder() :
	AbstractBuilder(std::ios::in | std::ios::binary, std::ios::out | std::ios::binary)
{
	m_operationsMap[ERROR_TOO_LITTLE_VEHICLE_BODY_EDGES] = "Error: Too little vehicle body edges!";
	m_operationsMap[ERROR_TOO_MANY_VEHICLE_BODY_EDGES] = "Error: Too many vehicle body edges!";
	m_operationsMap[ERROR_VEHICLE_BODY_AREA_IS_TOO_SMALL] = "Error: Vehicle body area is too small!";
	m_operationsMap[ERROR_VEHICLE_BODY_BOUNDARIES_ARE_TOO_VAST] = "Error: Vehicle body boundaries are too vast!";
	m_operationsMap[ERROR_TOO_LITTLE_SENSORS] = "Error: Too little sensors specified!";
	m_operationsMap[ERROR_TOO_MANY_SENSORS] = "Error: Too many sensors specified!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_NOT_DIVISIBLE] = "Error: Sensor's angle must be divisible by 15.0!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_TOO_LITTLE] = "Error: Sensor's angle is too little!";
	m_operationsMap[ERROR_SENSOR_ANGLE_IS_TOO_LARGE] = "Error: Sensor's angle is too large!";
	Clear();
}

DrawableVehicleBuilder::~DrawableVehicleBuilder()
{
}

void DrawableVehicleBuilder::AddVehicleBodyPoint(sf::Vector2f point)
{
	m_vehicleBody.AddPoint(point);
}

void DrawableVehicleBuilder::AddVehicleSensor(sf::Vector2f point, double angle)
{
	m_vehicleSensors.AddSensor(point, angle);
}

bool DrawableVehicleBuilder::CreateDummy()
{
	Clear();

	sf::Vector2f dummySize = GetMaxVehicleBodySize();
	dummySize /= 2.0f;

	m_vehicleBody = CreateVehicleBodyDummy(dummySize);

	m_vehicleSensors.AddSensor(sf::Vector2f(0, -dummySize.y / 2), 270.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, -dummySize.y / 2), 315.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, 0), 0);
	m_vehicleSensors.AddSensor(sf::Vector2f(dummySize.x / 2, dummySize.y / 2), 45.0);
	m_vehicleSensors.AddSensor(sf::Vector2f(0, dummySize.y / 2), 90.0);

	// Validate
	return Validate();
}

VehicleBody DrawableVehicleBuilder::GetVehicleBody()
{
	return m_vehicleBody;
}

VehicleSensors DrawableVehicleBuilder::GetVehicleSensors()
{
	return m_vehicleSensors;
}

sf::Vector2f DrawableVehicleBuilder::GetMaxVehicleBodySize()
{
	auto windowSize = CoreWindow::GetSize();
	const float widthFactor = 15.0f;
	const float heightFactor = 5;
	return sf::Vector2f(windowSize.y / heightFactor, windowSize.x / widthFactor);
}

size_t DrawableVehicleBuilder::GetMinVehicleBodyNumberOfEdges()
{
	return 3;
}

size_t DrawableVehicleBuilder::GetMaxVehicleBodyNumberOfEdges()
{
	return 12;
}

double DrawableVehicleBuilder::GetMinSensorAngle()
{
	return 0.0;
}

double DrawableVehicleBuilder::GetMaxSensorAngle()
{
	return 360.0;
}

size_t DrawableVehicleBuilder::GetMinNumberOfSensors()
{
	return ArtificialNeuralNetworkBuilder::GetMinNumberOfNeuronsPerLayer();
}

size_t DrawableVehicleBuilder::GetMaxNumberOfSensors()
{
	return ArtificialNeuralNetworkBuilder::GetMaxNumberOfNeuronsPerLayer();
}

double DrawableVehicleBuilder::GetDefaultSensorAngleOffset()
{
	return 15.0;
}

DrawableVehicle* DrawableVehicleBuilder::Get()
{
	if (!Validate())
		return nullptr;

	return new DrawableVehicle(m_vehicleBody, m_vehicleSensors);
}

