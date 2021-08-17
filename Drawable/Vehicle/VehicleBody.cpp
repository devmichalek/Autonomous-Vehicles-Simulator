#include "VehicleBody.hpp"
#include "CoreWindow.hpp"

VehicleBody::VehicleBody()
{
	m_vertices.setPrimitiveType(sf::Quads);
}

VehicleBody::~VehicleBody()
{
}

void VehicleBody::Update(const sf::Vector2f& center, const double& angle)
{
	double cosValue = cos(angle * M_PI / 180);
	double sinValue = sin(angle * M_PI / 180);

	// Update vertices position
	size_t count = m_vertices.getVertexCount();
	for (size_t i = 0; i < count; ++i)
	{
		float x = m_points[i].x;
		float y = m_points[i].y;
		m_vertices[i].position.x = static_cast<float>(double(x) * cosValue - double(y) * sinValue);
		m_vertices[i].position.y = static_cast<float>(double(x) * sinValue + double(y) * cosValue);
		m_vertices[i].position += center;
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

bool VehicleBody::Inside(const sf::Vector2f& point, const sf::Vector2f& center)
{
	float area = GetFigureArea(m_vertices, point);
	float correctArea = GetFigureArea(m_vertices, center) + 1;
	return area <= correctArea;
}

void VehicleBody::AddPoint(sf::Vector2f point)
{
	m_points.push_back(point);
	m_points.shrink_to_fit();
	m_vertices.resize(m_points.size());
}

void VehicleBody::RemovePoint()
{
	m_points.pop_back();
	m_points.shrink_to_fit();
	m_vertices.resize(m_points.size());
}