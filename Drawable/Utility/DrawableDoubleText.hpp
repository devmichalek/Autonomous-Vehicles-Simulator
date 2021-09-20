#pragma once
#include "DrawableTextAbstract.hpp"

class ObserverInterface;

class DrawableDoubleText :
	public DrawableTextAbstract
{
public:

	// <Consistent text> <Variable text>
	DrawableDoubleText(std::vector<std::string> strings, size_t size = 2);

	virtual ~DrawableDoubleText();

	// In case of existing observer sets text string and resets observer
	void Reset() override;

	// Sets consistent text and variable text positions
	// First component is used as y position for both texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	virtual void SetPosition(std::vector<FontContext::Component> components) override;

	// Sets observer for variable text string
	void SetObserver(ObserverInterface* observer);

protected:

	// If observer is ready text string is set
	virtual void UpdateInternal() override;

	// Sets second (variable) text color
	void SetVariableTextColor(sf::Color color = sf::Color(0xC0, 0xC0, 0xC0, 0xFF));

	enum
	{
		CONSISTENT_TEXT,
		VARIABLE_TEXT
	};

	ObserverInterface* m_observer;
};