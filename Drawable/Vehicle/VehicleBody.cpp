#include "VehicleBody.hpp"
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"

sf::Vector2f VehicleBody::m_baseCenter;
double VehicleBody::m_baseSinus;
double VehicleBody::m_baseCosinus;
bool VehicleBody::m_initialized = false;

VehicleBody::VehicleBody() :
	m_center(&m_baseCenter),
	m_sinus(&m_baseSinus),
	m_cosinus(&m_baseCosinus)
{
	if (!m_initialized)
		CoreLogger::PrintError("Vehicle body constructor has been called before initialization!");
	m_vertices.setPrimitiveType(sf::TriangleStrip);
}

VehicleBody::~VehicleBody()
{
}

void VehicleBody::Initialize()
{
	if (!m_initialized)
	{
		m_baseCenter = CoreWindow::GetCenter();
		m_baseSinus = sin(0);
		m_baseCosinus = cos(0);
		m_initialized = true;
	}
	else
		CoreLogger::PrintError("Vehicle sensors initialization was performed more than once!");
}

void VehicleBody::Clear()
{
	m_vertices.clear();
	m_points.clear();
}

void VehicleBody::SetBase(const sf::Vector2f* center,
						  const double* sinus,
						  const double* cosinus)
{
	m_center = center;
	m_sinus = sinus;
	m_cosinus = cosinus;
}

void VehicleBody::Update()
{
	// Update vertices position
	size_t count = m_vertices.getVertexCount();
	for (size_t i = 0; i < count; ++i)
	{
		float x = m_points[i].x;
		float y = m_points[i].y;
		m_vertices[i].position.x = static_cast<float>(double(x) * *m_cosinus - double(y) * *m_sinus);
		m_vertices[i].position.y = static_cast<float>(double(x) * *m_sinus + double(y) * *m_cosinus);
		m_vertices[i].position += *m_center;
	}
}

void VehicleBody::Draw()
{
	CoreWindow::GetRenderWindow().draw(m_vertices);
}

void VehicleBody::SetLeaderColor()
{
	size_t size = m_vertices.getVertexCount();
	for (size_t i = 0; i < size; ++i)
		m_vertices[i].color = sf::Color::Green;
}

void VehicleBody::SetFollowerColor()
{
	size_t size = m_vertices.getVertexCount();
	for (size_t i = 0; i < size; ++i)
		m_vertices[i].color = sf::Color::Yellow;
}

bool VehicleBody::Inside(const sf::Vector2f& point)
{
	for (size_t i = 2; i < m_vertices.getVertexCount(); ++i)
	{
		Triangle triangle = { m_vertices[i - 2].position, m_vertices[i - 1].position, m_vertices[i].position };
		if (DrawableMath::IsPointInsideTriangle(triangle, point))
		{
			return true;
		}
	}

	return false;
}

void VehicleBody::AddPoint(sf::Vector2f point)
{
	m_points.push_back(point);
	m_points.shrink_to_fit();
	m_vertices.resize(m_points.size());
}

void VehicleBody::RemovePoint(sf::Vector2f point)
{
	for (size_t i = 2; i < m_points.size(); ++i)
	{
		Triangle triangle = { m_points[i - 2], m_points[i - 1], m_points[i] };
		if (DrawableMath::IsPointInsideTriangle(triangle, point))
		{
			m_points.erase(m_points.begin() + i);
			if (m_points.size() < 3)
				m_points.clear();
			m_vertices.resize(m_points.size());
			break;
		}
	}
}

size_t VehicleBody::GetNumberOfPoints()
{
	return m_points.size();
}
