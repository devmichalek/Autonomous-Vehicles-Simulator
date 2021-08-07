#pragma once
#include "FontContext.hpp"

class Observer;

class DrawableDoubleText
{
public:

	DrawableDoubleText()
	{
		// Set font
		m_consistentText.setFont(FontContext::GetFont());
		m_variableText.setFont(FontContext::GetFont());

		// Set color
		m_consistentText.setFillColor(sf::Color::White);
		m_variableText.setFillColor(sf::Color(0xC0, 0xC0, 0xC0, 0xFF));

		// Set character size
		m_consistentText.setCharacterSize(FontContext::GetCharacterSize());
		m_variableText.setCharacterSize(FontContext::GetCharacterSize());

		m_observer = nullptr;
	}

	~DrawableDoubleText();

	void setConsistentText(std::string text);

	void setVariableText(std::string text);

	virtual void setPosition(double cx, double vx, double y);

	void setObserver(Observer* observer);

	virtual void update();

	virtual void draw();

protected:

	sf::Text m_consistentText;
	sf::Text m_variableText;
	sf::Vector2f m_consistentPosition;
	sf::Vector2f m_variablePosition;
	Observer* m_observer;
};