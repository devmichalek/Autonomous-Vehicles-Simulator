#include "ConsistentText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"

ConsistentText::ConsistentText(std::vector<std::string> strings) :
	AbstractText(strings, 1)
{
	m_texts.front().setFillColor(m_inactiveColor);
}

void ConsistentText::SetCharacterSize(unsigned int multiplier)
{
	m_texts[0].setCharacterSize(FontContext::GetCharacterSize(multiplier));
}

void ConsistentText::SetRotation(float rotation)
{
	m_texts[0].setRotation(rotation);
}

void ConsistentText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 2);

	float x = FontContext::CalculateRow(components[1]);
	float y = FontContext::CalculateColumn(components[0]);
	m_textPositions[0] = sf::Vector2f(x, y);

	Update();
}
