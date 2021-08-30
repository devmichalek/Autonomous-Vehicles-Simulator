#pragma once
#include "DrawableTextAbstract.hpp"

class ObserverIf;

class DrawableDoubleText :
	public DrawableTextAbstract
{
public:

	DrawableDoubleText(std::vector<std::string> strings, size_t size = 2);

	virtual ~DrawableDoubleText();

	void Reset() override;

	// Sets consistent text and variable text positions
	// First component is used as y position for both texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	virtual void SetPosition(std::vector<FontContext::Component> components) override;

	// Sets observer
	void SetObserver(ObserverIf* observer);

protected:

	virtual void UpdateInternal() override;

	void SetVariableTextColor(sf::Color color = sf::Color(0xC0, 0xC0, 0xC0, 0xFF));

	enum
	{
		CONSISTENT_TEXT,
		VARIABLE_TEXT
	};

	ObserverIf* m_observer;
};