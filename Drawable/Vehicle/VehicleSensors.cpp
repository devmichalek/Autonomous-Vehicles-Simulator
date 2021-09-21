#include "VehicleSensors.hpp"
#include "CoreLogger.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"

double VehicleSensors::m_beamLength;
sf::Vector2f VehicleSensors::m_sensorSize;
sf::Vector2f VehicleSensors::m_baseCenter;
double VehicleSensors::m_baseAngle;
double VehicleSensors::m_baseSinus;
double VehicleSensors::m_baseCosinus;
Line VehicleSensors::m_beamShape;
sf::CircleShape VehicleSensors::m_sensorShape;
std::mt19937 VehicleSensors::m_mersenneTwister((std::random_device())());
bool VehicleSensors::m_initialized = false;

VehicleSensors::VehicleSensors() :
	m_center(&m_baseCenter),
	m_angle(&m_baseAngle),
	m_sinus(&m_baseSinus),
	m_cosinus(&m_baseCosinus)
{
	if (!m_initialized)
		CoreLogger::PrintError("Vehicle sensors constructor has been called before initialization!");
}

VehicleSensors::~VehicleSensors()
{
}

void VehicleSensors::Initialize()
{
	if (!m_initialized)
	{
		auto windowSize = CoreWindow::GetSize();
		m_beamLength = double(windowSize.y) * 0.75;
		m_sensorSize.x = windowSize.x / 400.0f;
		m_sensorSize.y = m_sensorSize.x;
		m_baseCenter = CoreWindow::GetCenter();
		m_baseAngle = 0.0;
		m_baseSinus = sin(0);
		m_baseCosinus = cos(0);
		m_beamShape[0].color = sf::Color(255, 255, 255, 144);
		m_beamShape[1].color = sf::Color(255, 255, 255, 32);
		m_sensorShape.setRadius(m_sensorSize.x);
		m_sensorShape.setFillColor(sf::Color::Red);
		m_initialized = true;
	}
	else
		CoreLogger::PrintError("Vehicle sensors initialization was performed more than once!");
}

void VehicleSensors::Clear()
{
	m_beamVector.clear();
	m_points.clear();
	m_angleVector.clear();
	m_sensors.clear();
	m_motionRanges.clear();
}

void VehicleSensors::SetBase(const sf::Vector2f* center,
							 const double* angle,
							 const double* sinus,
							 const double* cosinus)
{
	m_center = center;
	m_angle = angle;
	m_sinus = sinus;
	m_cosinus = cosinus;
}

void VehicleSensors::Update()
{
	for (size_t i = 0; i < m_beamVector.size(); ++i)
	{
		// Set sensor max value
		m_sensors[i] = m_sensorMaxValue;

		// Set base
		m_beamVector[i][0] = m_points[i];

		// Update motion range
		m_motionRanges[i].Update();

		// Set beam start point
		float x = m_beamVector[i][0].x;
		float y = m_beamVector[i][0].y;
		m_beamVector[i][0].x = static_cast<float>(double(x) * *m_cosinus - double(y) * *m_sinus);
		m_beamVector[i][0].y = static_cast<float>(double(x) * *m_sinus + double(y) * *m_cosinus);
		m_beamVector[i][0] += *m_center;

		// Set beam end point
		auto cosBeam = cos((*m_angle + m_angleVector[i] + m_motionRanges[i].GetValue()) * M_PI / 180);
		auto sinBeam = sin((*m_angle + m_angleVector[i] + m_motionRanges[i].GetValue()) * M_PI / 180);
		m_beamVector[i][1].x = static_cast<float>(m_beamVector[i][0].x + m_beamLength * cosBeam);
		m_beamVector[i][1].y = static_cast<float>(m_beamVector[i][0].y + m_beamLength * sinBeam);
	}
}

void VehicleSensors::DrawBeams()
{
	for (const auto& beam : m_beamVector)
	{
		m_beamShape[0].position = beam[0];
		m_beamShape[1].position = beam[1];
		CoreWindow::GetRenderWindow().draw(m_beamShape.data(), m_beamShape.size(), sf::Lines);
	}
}

void VehicleSensors::DrawSensors()
{
	for (const auto& beam : m_beamVector)
	{
		m_sensorShape.setPosition(beam[0] - m_sensorSize);
		CoreWindow::GetRenderWindow().draw(m_sensorShape);
	}
}

void VehicleSensors::SetSensorAngle(size_t index, double angle)
{
	if (index < m_angleVector.size())
		m_angleVector[index] = angle;
	else
		CoreLogger::PrintError("Requested sensor index is outside of array bound!");
}

double VehicleSensors::GetSensorAngle(size_t index) const
{
	if (index < m_angleVector.size())
		return m_angleVector[index];
	
	CoreLogger::PrintError("Requested sensor index is outside of array bound!");
	return 0.0;
}

void VehicleSensors::SetSensorMotionRange(size_t index, double motionRange)
{
	auto& element = m_motionRanges[index];
	element.SetBoundaryValue(motionRange * 0.5);
	element.SetMultiplier(DrawableVehicleBuilder::GetSensorMotionRangeMultiplier());
	element.SetValue(GenerateMotionRangeValue(motionRange));
}

double VehicleSensors::GetSensorMotionRange(size_t index) const
{
	return m_motionRanges[index].GetValueRange();
}

double VehicleSensors::GenerateMotionRangeValue(double motionRange) const
{
	const double multiplier = 1000.0;
	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, unsigned(motionRange * multiplier));
	int result = distribution(m_mersenneTwister) - static_cast<int>(motionRange * 0.5 * multiplier);
	return double(result) / multiplier;
}

void VehicleSensors::AddSensor(sf::Vector2f point, double angle, double motionRange)
{
	// Push back
	m_beamVector.push_back(Edge());
	m_points.push_back(point);
	m_angleVector.push_back(angle);
	m_sensors.push_back(ArtificialNeuralNetworkBuilder::GetMinNeuronValue());
	m_motionRanges.emplace_back(motionRange * 0.5, DrawableVehicleBuilder::GetSensorMotionRangeMultiplier());
	m_motionRanges.back().SetValue(GenerateMotionRangeValue(motionRange));

	// Shrink to fit
	m_beamVector.shrink_to_fit();
	m_points.shrink_to_fit();
	m_angleVector.shrink_to_fit();
	m_sensors.shrink_to_fit();
	m_motionRanges.shrink_to_fit();
}

void VehicleSensors::RemoveSensor(size_t index)
{
	if (index < m_beamVector.size())
	{
		m_beamVector.erase(m_beamVector.begin() + index);
		m_points.erase(m_points.begin() + index);
		m_angleVector.erase(m_angleVector.begin() + index);
		m_sensors.erase(m_sensors.begin() + index);
		m_motionRanges.erase(m_motionRanges.begin() + index);
	}
	else
		CoreLogger::PrintError("Requested sensor index is outside of array bound!");
}

bool VehicleSensors::GetSensorIndex(size_t& index, sf::Vector2f point) const
{
	for (size_t i = 0; i < m_points.size(); ++i)
	{
		if (DrawableMath::IsPointInsideCircle(m_points[i], m_sensorSize.x, point))
		{
			index = i;
			return true;
		}
	}

	index = -1;
	return false;
}
