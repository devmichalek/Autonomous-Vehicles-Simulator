#include "StatusText.hpp"
#include "CoreLogger.hpp"

StatusText::StatusText(std::vector<std::string> strings) :
	TripleText(strings, 4),
	m_alphaTimer(0.0, 255.0, 75.0)
{
}

StatusText::~StatusText()
{
}

void StatusText::Reset()
{
	m_alphaTimer.MakeTimeout();
	TripleText::Reset();
}

void StatusText::SetErrorStatusText(std::string text)
{
	m_alphaTimer.Reset();
	TripleText::Reset();
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(ColorContext::ErrorText);
}

void StatusText::SetSuccessStatusText(std::string text)
{
	m_alphaTimer.Reset();
	TripleText::Reset();
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(ColorContext::SuccessText);
}

void StatusText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 5);

	float statusX = FontContext::CalculateRow(components[4]);
	float statusY = FontContext::CalculateColumn(components[0]);
	m_textPositions[STATUS_TEXT] = sf::Vector2f(statusX, statusY);
	components.pop_back();

	TripleText::SetPosition(components);

	Update();
}

void StatusText::UpdateInternal()
{
	m_alphaTimer.Update();
	auto alpha = ColorContext::MaxChannelValue - sf::Uint8(m_alphaTimer.GetValue());
	m_texts[STATUS_TEXT].setFillColor(ColorContext::Create(m_texts[STATUS_TEXT].getFillColor(), alpha));
	TripleText::UpdateInternal();
}
