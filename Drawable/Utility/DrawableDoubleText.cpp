#include "DrawableDoubleText.hpp"
#include "ObserverIf.hpp"
#include "CoreLogger.hpp"

DrawableDoubleText::DrawableDoubleText(std::vector<std::string> strings, size_t size) :
	DrawableTextAbstract(strings, size),
	m_observer(nullptr)
{
	SetVariableTextColor();
}

DrawableDoubleText::~DrawableDoubleText()
{
}

void DrawableDoubleText::Reset()
{
	UpdateInternal();
	if (m_observer)
		m_observer->Reset();
}

void DrawableDoubleText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 3);

	float consistentX = FontContext::CalculateRow(components[1]), consistentY = 0;
	float variableX = FontContext::CalculateRow(components[2]), variableY = 0;
	consistentY = variableY = FontContext::CalculateColumn(components[0]);
	m_textPositions[CONSISTENT_TEXT] = sf::Vector2f(consistentX, consistentY);
	m_textPositions[VARIABLE_TEXT] = sf::Vector2f(variableX, variableY);

	Update();
}

void DrawableDoubleText::SetObserver(ObserverIf* observer)
{
	m_observer = observer;
	Update();
}

void DrawableDoubleText::UpdateInternal()
{
	if (m_observer && m_observer->Ready())
		m_texts[VARIABLE_TEXT].setString(m_observer->Read());
}

void DrawableDoubleText::SetVariableTextColor(sf::Color color)
{
	m_texts[VARIABLE_TEXT].setFillColor(color);
}
