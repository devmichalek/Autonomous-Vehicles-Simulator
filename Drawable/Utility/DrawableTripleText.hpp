#pragma once
#include "DrawableDoubleText.hpp"

class Observer;

class DrawableTripleText :
	public DrawableDoubleText
{
public:

	DrawableTripleText();

	virtual ~DrawableTripleText();

	void SetInformationText(std::string text);

	// Sets consistent text, variable text and information text positions
	// First component is used as x position for consistent text
	// Second component is used as x position for variable text
	// Third component is used as x position for information text
	// Fourth component is used as y position for all texts
	virtual void SetPosition(std::array<FontContext::Component, 4> components);

	virtual void Update();

	virtual void Draw();

private:

	sf::Text m_informationText;
	sf::Vector2f m_informationPosition;
};