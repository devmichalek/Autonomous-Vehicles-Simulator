#include "DrawableVariableText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"

DrawableVariableText::DrawableVariableText(std::vector<std::string> strings) :
	DrawableTextAbstract(strings, 1)
{
}

DrawableVariableText::~DrawableVariableText()
{
}

void DrawableVariableText::Reset()
{

}

void DrawableVariableText::SetCharacterSize(unsigned int multiplier)
{
	m_texts[0].setCharacterSize(FontContext::GetCharacterSize(multiplier));
}

void DrawableVariableText::SetRotation(float rotation)
{
	m_texts[0].setRotation(rotation);
}

void DrawableVariableText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 2);

	float x = FontContext::CalculateRow(components[1]);
	float y = FontContext::CalculateColumn(components[0]);
	m_textPositions[0] = sf::Vector2f(x, y);

	Update();
}

void DrawableVariableText::UpdateInternal()
{
}
