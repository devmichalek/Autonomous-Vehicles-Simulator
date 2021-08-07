#pragma once
#include "DrawableTripleText.hpp"
#include "StoppableTimer.hpp"

class DrawableTripleTextStatus :
	public DrawableTripleText
{
public:

	DrawableTripleTextStatus() :
		DrawableTripleText(), m_alphaTimer(0.0, 255.0, 0.0, 75.0)
	{
		// Set font
		m_statusText.setFont(FontContext::GetFont());

		// Set color
		m_statusText.setFillColor(sf::Color::Red);

		// Set character size
		m_statusText.setCharacterSize(FontContext::GetCharacterSize());
	}

	void setStatusText(std::string text);

	void setStatusTextColor(sf::Color color);

	void showStatusText();

	void setPosition(double cx, double vx, double ix, double sx, double y);

	void update();

	void draw();

private:

	sf::Text m_statusText;
	sf::Vector2f m_statusPosition;
	StoppableTimer m_alphaTimer;
};