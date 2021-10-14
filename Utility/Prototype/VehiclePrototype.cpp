#include "VehiclePrototype.hpp"
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"
#include "VehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"

VehiclePrototype::VehiclePrototype() :
	m_center(CoreWindow::GetSize() / 2.f),
	m_angle(0.0)
{
	m_bodyShape.setPointCount(0);
	m_beamShape[0].color = sf::Color(255, 255, 255, 144);
	m_beamShape[1].color = sf::Color(255, 255, 255, 32);
	m_sensorShape.setRadius(VehicleBuilder::GetDefaultSensorSize().x);
	m_sensorShape.setFillColor(sf::Color(0xAA, 0x4A, 0x44, 0xFF));
	Update();
}

VehiclePrototype::~VehiclePrototype()
{
}

void VehiclePrototype::Clear()
{
	m_bodyPoints.clear();
	m_sensorPoints.clear();
	m_beamVector.clear();
	m_beamAngles.clear();
	m_motionRanges.clear();
	m_bodyShape.setPointCount(0);
}

void VehiclePrototype::AddBodyPoint(sf::Vector2f point)
{
	m_bodyPoints.push_back(point);
	m_bodyPoints.shrink_to_fit();
	m_bodyShape.setPointCount(m_bodyPoints.size());
	m_bodyShape.setPoint(m_bodyPoints.size() - 1, point);
}

void VehiclePrototype::RemoveLastBodyPoint()
{
	if (GetNumberOfBodyPoints() == 3)
		Clear();
	else if (GetNumberOfBodyPoints() != 0)
	{
		m_bodyPoints.pop_back();
		m_bodyShape.setPointCount(m_bodyPoints.size());
	}
}

sf::Vector2f VehiclePrototype::GetBodyPoint(size_t index) const
{
	if (index < m_bodyPoints.size())
		return m_bodyPoints[index];
	
	CoreLogger::PrintError("Requested body point is outside of array bound!");
	return sf::Vector2f(0.f, 0.f);
}

void VehiclePrototype::SetSensorBeamAngle(size_t index, double angle)
{
	if (index < m_beamAngles.size())
		m_beamAngles[index] = angle;
	else
		CoreLogger::PrintError("Requested sensor beam angle index is outside of array bound!");
}

double VehiclePrototype::GetSensorBeamAngle(size_t index) const
{
	if (index < m_beamAngles.size())
		return m_beamAngles[index];

	CoreLogger::PrintError("Requested sensor beam angle index is outside of array bound!");
	return 0.0;
}

void VehiclePrototype::AddSensor(sf::Vector2f point, double angle, double motionRange)
{
	// Push back
	m_beamVector.push_back(Edge());
	m_sensorPoints.push_back(point);
	m_beamAngles.push_back(angle);
	m_motionRanges.emplace_back(motionRange * 0.5, VehicleBuilder::GetSensorMotionRangeMultiplier());
	m_motionRanges.back().SetValue(GenerateMotionRangeValue(motionRange));

	// Shrink to fit
	m_beamVector.shrink_to_fit();
	m_sensorPoints.shrink_to_fit();
	m_beamAngles.shrink_to_fit();
	m_motionRanges.shrink_to_fit();
}

void VehiclePrototype::RemoveSensor(size_t index)
{
	if (index < m_beamVector.size())
	{
		m_beamVector.erase(m_beamVector.begin() + index);
		m_sensorPoints.erase(m_sensorPoints.begin() + index);
		m_beamAngles.erase(m_beamAngles.begin() + index);
		m_motionRanges.erase(m_motionRanges.begin() + index);
	}
	else
		CoreLogger::PrintError("Requested sensor index is outside of array bound!");
}

bool VehiclePrototype::GetSensorIndex(size_t& index, sf::Vector2f point) const
{
	for (size_t i = 0; i < m_sensorPoints.size(); ++i)
	{
		if (DrawableMath::IsPointInsideCircle(m_sensorPoints[i], VehicleBuilder::GetDefaultSensorSize().x, point))
		{
			index = i;
			return true;
		}
	}

	index = -1;
	return false;
}

sf::Vector2f VehiclePrototype::GetSensorPoint(size_t index)
{
	if (index < m_sensorPoints.size())
		return m_sensorPoints[index];

	CoreLogger::PrintError("Requested sensor point is outside of array bound!");
	return sf::Vector2f(0.f, 0.f);
}

void VehiclePrototype::SetSensorMotionRange(size_t index, double motionRange)
{
	if (index < m_motionRanges.size())
	{
		auto& element = m_motionRanges[index];
		element.SetBoundaryValue(motionRange * 0.5);
		element.SetMultiplier(VehicleBuilder::GetSensorMotionRangeMultiplier());
		element.SetValue(GenerateMotionRangeValue(motionRange));
	}
	else
		CoreLogger::PrintError("Requested sensor motion range index is outside of array bound!");
}

double VehiclePrototype::GetSensorMotionRange(size_t index) const
{
	if (index < m_motionRanges.size())
		return m_motionRanges[index].GetValueRange();

	CoreLogger::PrintError("Requested sensor motion range is outside of array bound!");
	return 0.0;
}

double VehiclePrototype::GenerateMotionRangeValue(double motionRange) const
{
	const double multiplier = 1000.0;
	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, unsigned(motionRange * multiplier));
	int result = distribution(CoreWindow::GetMersenneTwister()) - static_cast<int>(motionRange * 0.5 * multiplier);
	return double(result) / multiplier;
}

void VehiclePrototype::Update()
{
	double cosinus = cos(m_angle * M_PI / 180);
	double sinus = sin(m_angle * M_PI / 180);

	// Update vehicle body
	size_t count = m_bodyPoints.size();
	for (size_t i = 0; i < count; ++i)
	{
		double x = double(m_bodyPoints[i].x);
		double y = double(m_bodyPoints[i].y);
		float calculatedX = static_cast<float>(x * cosinus - y * sinus) + m_center.x;
		float calculatedY = static_cast<float>(x * sinus + y * cosinus) + m_center.y;
		m_bodyShape.setPoint(i, sf::Vector2f(calculatedX, calculatedY));
	}

	// Update vehicle sensors
	for (size_t i = 0; i < m_beamVector.size(); ++i)
	{
		// Set base
		m_beamVector[i][0] = m_sensorPoints[i];

		// Update motion range
		m_motionRanges[i].Update();

		// Set beam start point
		float x = m_beamVector[i][0].x;
		float y = m_beamVector[i][0].y;
		m_beamVector[i][0].x = static_cast<float>(double(x) * cosinus - double(y) * sinus);
		m_beamVector[i][0].y = static_cast<float>(double(x) * sinus + double(y) * cosinus);
		m_beamVector[i][0] += m_center;

		// Set beam end point
		auto cosBeam = cos((m_angle + m_beamAngles[i] + m_motionRanges[i].GetValue()) * M_PI / 180);
		auto sinBeam = sin((m_angle + m_beamAngles[i] + m_motionRanges[i].GetValue()) * M_PI / 180);
		m_beamVector[i][1].x = static_cast<float>(m_beamVector[i][0].x + VehicleBuilder::GetDefaultBeamLength() * cosBeam);
		m_beamVector[i][1].y = static_cast<float>(m_beamVector[i][0].y + VehicleBuilder::GetDefaultBeamLength() * sinBeam);
	}
}

void VehiclePrototype::DrawBody()
{
	CoreWindow::GetRenderWindow().draw(m_bodyShape);
	for (const auto& beam : m_beamVector)
	{
		m_sensorShape.setPosition(beam[0] - VehicleBuilder::GetDefaultSensorSize());
		CoreWindow::GetRenderWindow().draw(m_sensorShape);
	}
}

void VehiclePrototype::DrawBeams()
{
	for (const auto& beam : m_beamVector)
	{
		m_beamShape[0].position = beam[0];
		m_beamShape[1].position = beam[1];
		CoreWindow::GetRenderWindow().draw(m_beamShape.data(), m_beamShape.size(), sf::Lines);
	}
}
