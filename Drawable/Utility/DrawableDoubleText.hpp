#pragma once
#include "FontContext.hpp"
#include <array>

class ObserverIf;

class DrawableDoubleText
{
public:

	DrawableDoubleText();

	virtual ~DrawableDoubleText();

	void SetConsistentText(std::string text);

	void SetVariableText(std::string text);

	void SetVariableTextColor(sf::Color color = sf::Color(0xC0, 0xC0, 0xC0, 0xFF));

	// Sets consistent text and variable text positions
	// First component is used as x position for consistent text
	// Second component is used as x position for variable text
	// Third component is used as y position for both texts
	virtual void SetPosition(std::array<FontContext::Component, 3> components);

	void SetObserver(ObserverIf* observer);

	void ResetObserverTimer();

	virtual void Update();

	virtual void Draw();

protected:

	sf::Text m_consistentText;
	sf::Text m_variableText;
	sf::Vector2f m_consistentPosition;
	sf::Vector2f m_variablePosition;
	ObserverIf* m_observer;
};