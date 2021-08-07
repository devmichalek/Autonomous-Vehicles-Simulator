#pragma once
#include "DrawableDoubleText.hpp"

class Observer;

class DrawableTripleText :
	public DrawableDoubleText
{
public:

	DrawableTripleText() :
		DrawableDoubleText()
	{
		// Set font
		m_informationText.setFont(FontContext::GetFont());

		// Set color
		m_informationText.setFillColor(sf::Color::White);

		// Set character size
		m_informationText.setCharacterSize(FontContext::GetCharacterSize());
	}

	void setInformationText(std::string text);

	virtual void setPosition(double cx, double vx, double ix, double y);

	virtual void update();

	virtual void draw();

protected:

	sf::Text m_informationText;
	sf::Vector2f m_informationPosition;
};