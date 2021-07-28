#include "DrawableTripleTextStatus.hpp"
#include "Observer.hpp"
#include "CoreWindow.hpp"

void DrawableTripleTextStatus::setStatusText(std::string text)
{
	m_statusText.setString(text);
}

void DrawableTripleTextStatus::setStatusTextColor(sf::Color color)
{
	m_statusText.setFillColor(color);
}

void DrawableTripleTextStatus::showStatusText()
{
	m_alphaTimer.maximize();
}

void DrawableTripleTextStatus::setPosition(double cx, double vx, double ix, double sx, double y)
{
	auto windowSize = CoreWindow::getSize();
	float consistentX = float(double(windowSize.x) * cx);
	float consistentY = float(double(windowSize.y) * y);
	float variableX = float(double(windowSize.x) * vx);
	float variableY = float(double(windowSize.y) * y);
	float informationX = float(double(windowSize.x) * ix);
	float informationY = float(double(windowSize.y) * y);
	float statusX = float(double(windowSize.x) * sx);
	float statusY = float(double(windowSize.y) * y);
	m_consistentPosition = sf::Vector2f(consistentX, consistentY);
	m_variablePosition = sf::Vector2f(variableX, variableY);
	m_informationPosition = sf::Vector2f(informationX, informationY);
	m_statusPosition = sf::Vector2f(statusX, statusY);
	update();
}

void DrawableTripleTextStatus::update()
{
	if (m_observer && m_observer->timeout())
		m_variableText.setString(m_observer->read());

	sf::Color color = m_statusText.getFillColor();
	m_alphaTimer.decrement();
	color.a = static_cast<sf::Uint8>(m_alphaTimer.value());
	m_statusText.setFillColor(color);

	sf::Vector2f viewOffset = CoreWindow::getViewOffset();
	m_consistentText.setPosition(m_consistentPosition + viewOffset);
	m_variableText.setPosition(m_variablePosition + viewOffset);
	m_informationText.setPosition(m_informationPosition + viewOffset);
	m_statusText.setPosition(m_statusPosition + viewOffset);
}

void DrawableTripleTextStatus::draw()
{
	CoreWindow::getRenderWindow().draw(m_consistentText);
	CoreWindow::getRenderWindow().draw(m_variableText);
	CoreWindow::getRenderWindow().draw(m_informationText);
	CoreWindow::getRenderWindow().draw(m_statusText);
}