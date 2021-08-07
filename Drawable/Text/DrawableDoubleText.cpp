#include "DrawableDoubleText.hpp"
#include "CoreWindow.hpp"
#include "Observer.hpp"

DrawableDoubleText::~DrawableDoubleText()
{
	delete m_observer;
}

void DrawableDoubleText::setConsistentText(std::string text)
{
	m_consistentText.setString(text);
}

void DrawableDoubleText::setVariableText(std::string text)
{
	m_variableText.setString(text);
}

void DrawableDoubleText::setPosition(double cx, double vx, double y)
{
	auto windowSize = CoreWindow::GetSize();
	float consistentX = float(double(windowSize.x) * cx);
	float consistentY = float(double(windowSize.y) * y);
	float variableX = float(double(windowSize.x) * vx);
	float variableY = float(double(windowSize.y) * y);
	m_consistentPosition = sf::Vector2f(consistentX, consistentY);
	m_variablePosition = sf::Vector2f(variableX, variableY);
	update();
}

void DrawableDoubleText::setObserver(Observer* observer)
{
	m_observer = observer;
	update();
}

void DrawableDoubleText::update()
{
	if (m_observer && m_observer->timeout())
		m_variableText.setString(m_observer->read());

	sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
	m_consistentText.setPosition(m_consistentPosition + viewOffset);
	m_variableText.setPosition(m_variablePosition + viewOffset);
}

void DrawableDoubleText::draw()
{
	CoreWindow::GetRenderWindow().draw(m_consistentText);
	CoreWindow::GetRenderWindow().draw(m_variableText);
}
