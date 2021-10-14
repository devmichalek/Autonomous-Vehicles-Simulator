#include "DoubleText.hpp"
#include "ObserverInterface.hpp"
#include "CoreLogger.hpp"

DoubleText::DoubleText(std::vector<std::string> strings, size_t size) :
	TextAbstract(strings, size),
	m_observer(nullptr)
{
	SetVariableTextColor();
}

DoubleText::~DoubleText()
{
}

void DoubleText::Reset()
{
	UpdateInternal();
	if (m_observer)
		m_observer->Reset();
}

void DoubleText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 3);

	float consistentX = FontContext::CalculateRow(components[1]), consistentY = 0;
	float variableX = FontContext::CalculateRow(components[2]), variableY = 0;
	consistentY = variableY = FontContext::CalculateColumn(components[0]);
	m_textPositions[CONSISTENT_TEXT] = sf::Vector2f(consistentX, consistentY);
	m_textPositions[VARIABLE_TEXT] = sf::Vector2f(variableX, variableY);

	Update();
}

void DoubleText::SetObserver(ObserverInterface* observer)
{
	m_observer = observer;
	Update();
}

void DoubleText::UpdateInternal()
{
	if (m_observer && m_observer->Ready())
		m_texts[VARIABLE_TEXT].setString(m_observer->Read());
}

void DoubleText::SetVariableTextColor(sf::Color color)
{
	m_texts[VARIABLE_TEXT].setFillColor(color);
}
