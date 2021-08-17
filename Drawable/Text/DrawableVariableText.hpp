#pragma once
#include "FontContext.hpp"
#include <array>

class DrawableVariableText
{
public:

	DrawableVariableText();

	virtual ~DrawableVariableText();

	void SetText(std::string text);

	void SetTextColor(sf::Color color = sf::Color::White);

	void SetCharacterSize(unsigned int multiplier = 1);

	void SetRotation(float rotation);

	// Sets consistent text and variable text positions
	// First component is used as x position for the text
	// Second component is used as y position for the text
	virtual void SetPosition(std::array<FontContext::Component, 2> components);

	virtual void Update();

	virtual void Draw();

protected:

	sf::Text m_text;
	sf::Vector2f m_position;
};