#include "DrawableStatusText.hpp"
#include "CoreLogger.hpp"

DrawableStatusText::DrawableStatusText(std::vector<std::string> strings) :
	DrawableTripleText(strings, 4),
	m_alphaTimer(0.0, 255.0, 75.0)
{
}

DrawableStatusText::~DrawableStatusText()
{
}

void DrawableStatusText::Reset()
{
	m_alphaTimer.MakeTimeout();
	DrawableTripleText::Reset();
}

void DrawableStatusText::SetErrorStatusText(std::string text)
{
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(sf::Color::Red);
}

void DrawableStatusText::SetSuccessStatusText(std::string text)
{
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(sf::Color::Green);
}

void DrawableStatusText::ShowStatusText()
{
	m_alphaTimer.Reset();
	DrawableTripleText::Reset();
}

void DrawableStatusText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 5);

	float statusX = FontContext::CalculateRow(components[4]);
	float statusY = FontContext::CalculateColumn(components[0]);
	m_textPositions[STATUS_TEXT] = sf::Vector2f(statusX, statusY);
	components.pop_back();

	DrawableTripleText::SetPosition(components);

	Update();
}

void DrawableStatusText::UpdateInternal()
{
	m_alphaTimer.Update();
	sf::Color color = m_texts[STATUS_TEXT].getFillColor();
	color.a = static_cast<sf::Uint8>(255.0 - m_alphaTimer.GetValue());
	m_texts[STATUS_TEXT].setFillColor(color);

	DrawableTripleText::UpdateInternal();
}
