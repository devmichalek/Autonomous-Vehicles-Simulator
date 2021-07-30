#include "DrawableTripleText.hpp"
#include "CoreWindow.hpp"
#include "Observer.hpp"

void DrawableTripleText::setInformationText(std::string text)
{
	m_informationText.setString(text);
}

void DrawableTripleText::setPosition(double cx, double vx, double ix, double y)
{
	auto windowSize = CoreWindow::getSize();
	float consistentX = float(double(windowSize.x) * cx);
	float consistentY = float(double(windowSize.y) * y);
	float variableX = float(double(windowSize.x) * vx);
	float variableY = float(double(windowSize.y) * y);
	float informationX = float(double(windowSize.x) * ix);
	float informationY = float(double(windowSize.y) * y);
	m_consistentPosition = sf::Vector2f(consistentX, consistentY);
	m_variablePosition = sf::Vector2f(variableX, variableY);
	m_informationPosition = sf::Vector2f(informationX, informationY);
	update();
}

void DrawableTripleText::update()
{
	if (m_observer && m_observer->timeout())
		m_variableText.setString(m_observer->read());

	sf::Vector2f viewOffset = CoreWindow::getViewOffset();
	m_consistentText.setPosition(m_consistentPosition + viewOffset);
	m_variableText.setPosition(m_variablePosition + viewOffset);
	m_informationText.setPosition(m_informationPosition + viewOffset);
}

void DrawableTripleText::draw()
{
	CoreWindow::getRenderWindow().draw(m_consistentText);
	CoreWindow::getRenderWindow().draw(m_variableText);
	CoreWindow::getRenderWindow().draw(m_informationText);
}