#include "DrawableVariableText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"

DrawableVariableText::DrawableVariableText()
{
	// Set font
	m_text.setFont(FontContext::GetFont());

	// Set text color
	SetTextColor();

	// Set text character size
	SetCharacterSize();
}

DrawableVariableText::~DrawableVariableText()
{
}

void DrawableVariableText::SetText(std::string text)
{
	m_text.setString(text);
}

void DrawableVariableText::SetTextColor(sf::Color color)
{
	m_text.setFillColor(color);
}

void DrawableVariableText::SetCharacterSize(unsigned int multiplier)
{
	m_text.setCharacterSize(FontContext::GetCharacterSize(multiplier));
}

void DrawableVariableText::SetRotation(float rotation)
{
	m_text.setRotation(rotation);
}

void DrawableVariableText::SetPosition(std::array<FontContext::Component, 2> components)
{
	float x = FontContext::CalculateRow(components[0]);
	float y = FontContext::CalculateColumn(components[1]);
	m_position = sf::Vector2f(x, y);
	Update();
}

void DrawableVariableText::Update()
{
	sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
	m_text.setPosition(m_position + viewOffset);
}

void DrawableVariableText::Draw()
{
	CoreWindow::GetRenderWindow().draw(m_text);
}
